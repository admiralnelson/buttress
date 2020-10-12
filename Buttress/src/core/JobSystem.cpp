#include "pch.h"
#include "core/JobSystem.h"

/**
 * Global system components.
 */
static std::atomic<size_t>             s_nextJobId;        ///< Job ID assignment for debugging / profiling.
static std::mutex                      s_signalLock;       ///< Global mutex for worker signaling.
static std::condition_variable         s_signalThreads;    ///< Global condition var for worker signaling.
static std::atomic<size_t>             s_activeWorkers;

void jobsystem::JobState::SetQueued()
{
    m_done.store(false, std::memory_order_release);
}

void jobsystem::JobState::SetDone()
{
    JOBSYSTEM_ASSERT(!IsDone());

    for (const JobStatePtr& dependant : m_dependants)
    {
        dependant->m_dependencies.fetch_sub(1, std::memory_order_relaxed);
    }

    std::lock_guard<std::mutex> lock(m_doneMutex);
    m_done.store(true, std::memory_order_release);
    m_doneSignal.notify_all();
}

bool jobsystem::JobState::AwaitingCancellation() const
{
    return m_cancel.load(std::memory_order_relaxed);
}

jobsystem::JobState::JobState() : m_debugChar(0)
{
    m_jobId = s_nextJobId++;
    m_workerAffinity = kAffinityAllBits;

    m_dependencies.store(0, std::memory_order_release);
    m_cancel.store(false, std::memory_order_release);
    m_ready.store(false, std::memory_order_release);
    m_done.store(false, std::memory_order_release);
}

jobsystem::JobState& jobsystem::JobState::SetReady()
{
    JOBSYSTEM_ASSERT(!IsDone());

    m_cancel.store(false, std::memory_order_relaxed);
    m_ready.store(true, std::memory_order_release);

    s_signalThreads.notify_all();

    return *this;
}

jobsystem::JobState& jobsystem::JobState::Cancel()
{
    JOBSYSTEM_ASSERT(!IsDone());

    m_cancel.store(true, std::memory_order_relaxed);

    return *this;
}

jobsystem::JobState& jobsystem::JobState::AddDependant(JobStatePtr dependant)
{
    JOBSYSTEM_ASSERT(m_dependants.end() == std::find(m_dependants.begin(), m_dependants.end(), dependant));

    m_dependants.push_back(dependant);

    dependant->m_dependencies.fetch_add(1, std::memory_order_relaxed);

    return *this;
}

jobsystem::JobState& jobsystem::JobState::SetWorkerAffinity(affinity_t affinity)
{
    m_workerAffinity = affinity ? affinity : kAffinityAllBits;

    return *this;
}

bool jobsystem::JobState::IsDone() const
{
    return m_done.load(std::memory_order_acquire);
}

bool jobsystem::JobState::Wait(size_t maxWaitMicroseconds)
{
    if (!IsDone())
    {
        std::unique_lock<std::mutex> lock(m_doneMutex);

        if (maxWaitMicroseconds == 0)
        {
            m_doneSignal.wait(lock,
                [this]()
            {
                return IsDone();
            }
            );
        }
        else
        {
            m_doneSignal.wait_for(lock, std::chrono::microseconds(maxWaitMicroseconds));
        }
    }

    return IsDone();
}

bool jobsystem::JobState::AreDependenciesMet() const
{
    if (!m_ready.load(std::memory_order_acquire))
    {
        return false;
    }

    if (m_dependencies.load(std::memory_order_relaxed) > 0)
    {
        return false;
    }

    return true;
}

bool jobsystem::JobState::HasDependencies() const
{
    return (m_dependencies.load(std::memory_order_relaxed) > 0);
}

void jobsystem::JobSystemWorker::Start(size_t index, JobSystemWorker** allWorkers, size_t workerCount)
{
    m_allWorkers = allWorkers;
    m_workerCount = (NrOfThreads)workerCount;
    m_workerIndex = (ThreadNr)index;

    m_thread = std::thread(&JobSystemWorker::WorkerThreadProc, this);
}

void jobsystem::JobSystemWorker::Shutdown()
{
    m_stop.store(true, std::memory_order_relaxed);

    while (!m_hasShutDown.load(std::memory_order_acquire))
    {
        s_signalThreads.notify_all();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    if (m_hasShutDown.load(std::memory_order_acquire))
    {
        m_thread.join();
    }
}

jobsystem::JobStatePtr jobsystem::JobSystemWorker::PushJob(JobDelegate delegate)
{
    JobQueueEntry entry = { delegate, std::make_shared<JobState>() };
    entry.m_state->SetQueued();

    {
        std::lock_guard<std::mutex> queueLock(m_queueLock);
        m_queue.insert(m_queue.begin(), entry);
    }

    return entry.m_state;
}

void jobsystem::JobSystemWorker::NotifyEventObserver(const JobQueueEntry& job, EJobEvent event, uint64_t workerIndex, size_t jobId)
{
#ifdef JOBSYSTEM_ENABLE_PROFILING

    if (m_eventObserver)
    {
        m_eventObserver(job, event, workerIndex, jobId);
    }
#endif // JOBSYSTEM_ENABLE_PROFILING
    job;
    event;
    workerIndex;
    jobId;
}

bool jobsystem::JobSystemWorker::PopJobFromQueue(JobQueue& queue, JobQueueEntry& job, bool& hasUnsatisfiedDependencies, affinity_t workerAffinity)
{
    for (auto jobIter = queue.begin(); jobIter != queue.end();)
    {
        const JobQueueEntry& candidate = (*jobIter);

        if ((workerAffinity & candidate.m_state->m_workerAffinity) != 0)
        {
            if (candidate.m_state->AwaitingCancellation())
            {
                candidate.m_state->SetDone();
                jobIter = queue.erase(jobIter);

                continue;
            }
            else if (candidate.m_state->AreDependenciesMet())
            {
                job = candidate;
                job.m_state->m_threadCounts = m_workerCount;
                job.m_state->m_threadNr = m_workerIndex;
                queue.erase(jobIter);

                NotifyEventObserver(job, EJobEvent::eJobEvent_JobPopped, m_workerIndex);

                return true;
            }
        }

        hasUnsatisfiedDependencies = true;
        ++jobIter;
    }

    return false;
}

bool jobsystem::JobSystemWorker::PopNextJob(JobQueueEntry& job, bool& hasUnsatisfiedDependencies, bool useWorkStealing, affinity_t workerAffinity)
{
    bool foundJob = false;

    {
        std::lock_guard<std::mutex> queueLock(m_queueLock);
        foundJob = PopJobFromQueue(m_queue, job, hasUnsatisfiedDependencies, workerAffinity);
    }

    if (!foundJob && useWorkStealing)
    {
        for (size_t i = 0; foundJob == false && i < m_workerCount; ++i)
        {
            JOBSYSTEM_ASSERT(m_allWorkers[i]);
            JobSystemWorker& worker = *m_allWorkers[i];

            {
                std::lock_guard<std::mutex> queueLock(worker.m_queueLock);
                foundJob = PopJobFromQueue(worker.m_queue, job, hasUnsatisfiedDependencies, workerAffinity);
            }
        }

        if (foundJob)
        {
            NotifyEventObserver(job, EJobEvent::eJobEvent_JobStolen, m_workerIndex);
        }
    }

    return foundJob;
}

void jobsystem::JobSystemWorker::SetThreadName(const char* name) 
{
    (void)name;
#if defined(WINDOWS)
    typedef struct tagTHREADNAME_INFO
    {
        unsigned long dwType; // must be 0x1000
        const char* szName; // pointer to name (in user addr space)
        unsigned long dwThreadID; // thread ID (-1=caller thread)
        unsigned long dwFlags; // reserved for future use, must be zero
    } THREADNAME_INFO;

    THREADNAME_INFO threadName;
    threadName.dwType = 0x1000;
    threadName.szName = name;
    threadName.dwThreadID = GetCurrentThreadId();
    threadName.dwFlags = 0;
    __try
    {
        RaiseException(0x406D1388, 0, sizeof(threadName) / sizeof(ULONG_PTR), (ULONG_PTR*)&threadName);
    }
    __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
#elif defined(LINUX)
    pthread_setname_np(pthread_self(), name);
#endif
}

void jobsystem::JobSystemWorker::WorkerThreadProc()
{
    SetThreadName(m_desc.m_name.c_str());

#if defined(WINDOWS)
    SetThreadAffinityMask(m_thread.native_handle(), m_desc.m_cpuAffinity);
#elif defined(LINUX)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (size_t i = 0; i < sizeof(m_desc.m_cpuAffinity) * 8; ++i)
    {
        if ((1 << i) & m_desc.m_cpuAffinity)
        {
            CPU_SET(i, &cpuset);
        }
    }
#endif

    const affinity_t workerAffinity = CalculateSafeWorkerAffinity(m_workerIndex, m_workerCount);

    while (true)
    {
        JobQueueEntry job;
        {
            std::unique_lock<std::mutex> signalLock(s_signalLock);

            bool hasUnsatisfiedDependencies;

            while (!m_stop.load(std::memory_order_relaxed) &&
                !PopNextJob(job, hasUnsatisfiedDependencies, m_desc.m_enableWorkStealing, workerAffinity))
            {
                s_signalThreads.wait(signalLock);
                NotifyEventObserver(job, EJobEvent::eJobEvent_WorkerAwoken, m_workerIndex);
            }
        }

        if (m_stop.load(std::memory_order_relaxed))
        {
            m_hasShutDown.store(true, std::memory_order_release);

            break;
        }

        s_activeWorkers.fetch_add(1, std::memory_order_acq_rel);
        {
            NotifyEventObserver(job, EJobEvent::eJobEvent_WorkerUsed, m_workerIndex);

            NotifyEventObserver(job, EJobEvent::eJobEvent_JobStart, m_workerIndex, job.m_state->m_jobId);
            job.m_delegate(m_workerIndex, m_workerCount);
            NotifyEventObserver(job, EJobEvent::eJobEvent_JobDone, m_workerIndex);

            job.m_state->SetDone();

            NotifyEventObserver(job, EJobEvent::eJobEvent_JobRun, m_workerIndex);

            s_signalThreads.notify_one();
        }
        s_activeWorkers.fetch_sub(1, std::memory_order_acq_rel);
    }
}

void jobsystem::JobManager::Observer(const JobQueueEntry& job, EJobEvent event, uint64_t workerIndex, size_t jobId)

{
#ifdef JOBSYSTEM_ENABLE_PROFILING
    switch (event)
    {
    case eJobEvent_JobRun:
    {
        ++m_jobsRun;
    }
    break;

    case eJobEvent_JobStolen:
    {
        ++m_jobsStolen;
    }
    break;

    case eJobEvent_JobRunAssisted:
    {
        ++m_jobsAssisted;
        ++m_jobsRun;
    }
    break;

    case eJobEvent_WorkerAwoken:
    {
        m_awokenMask |= GetBit(workerIndex);
    }
    break;

    case eJobEvent_WorkerUsed:
    {
        m_usedMask |= GetBit(workerIndex);
    }
    break;

    case eJobEvent_JobStart:
    {
        ProfilingTimeline& timeline = workerIndex < m_workers.size() ? m_timelines[workerIndex] : m_timelines[m_workers.size()];
        ProfilingTimeline::TimelineEntry entry;
        entry.jobId = jobId;
        entry.start = ProfileClockNow();
        entry.debugChar = job.m_state ? job.m_state->m_debugChar : 0;
        timeline.m_entries.push_back(entry);
    }
    break;

    case eJobEvent_JobDone:
    {
        ProfilingTimeline& timeline = workerIndex < m_workers.size() ? m_timelines[workerIndex] : m_timelines[m_workers.size()];
        ProfilingTimeline::TimelineEntry& entry = timeline.m_entries.back();
        entry.end = ProfileClockNow();
    }
    break;

    case eJobEvent_JobPopped:
    {
        if (!m_hasPushedJob)
        {
            m_firstJobTime = ProfileClockNow();
            m_hasPushedJob = true;
        }
    }
    break;
    }
#endif // JOBSYSTEM_ENABLE_PROFILING
    job; event; workerIndex; jobId;
}

jobsystem::JobManager::~JobManager()
{
    DumpProfilingResults();

    JoinWorkersAndShutdown();
}

bool jobsystem::JobManager::Create(const JobManagerDescriptor& desc)
{
    JoinWorkersAndShutdown();

    m_desc = desc;

    const size_t workerCount = desc.m_workers.size();
    m_workers.reserve(workerCount);

#ifdef JOBSYSTEM_ENABLE_PROFILING

    m_timelines = new ProfilingTimeline[workerCount + 1];
    m_hasPushedJob = false;

#endif // JOBSYSTEM_ENABLE_PROFILING

    const JobEventObserver observer = std::bind(
        &JobManager::Observer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    // Create workers. We don't spawn the threads yet.
    for (size_t i = 0; i < workerCount; ++i)
    {
        const JobWorkerDescriptor& workerDesc = desc.m_workers[i];

        JobSystemWorker* worker = new JobSystemWorker(workerDesc, observer);
        m_workers.push_back(worker);
    }

    // Start the workers (includes spawning threads). Each worker maintains
    // understanding of what other workers exist, for work-stealing purposes.
    for (size_t i = 0; i < workerCount; ++i)
    {
        m_workers[i]->Start(i, &m_workers[0], workerCount);
    }

    return !m_workers.empty();
}

jobsystem::JobStatePtr jobsystem::JobManager::AddJob(JobDelegate delegate, char debugChar)
{
    JobStatePtr state = nullptr;

    // \todo - workers should maintain a tls pointer to themselves, so we can push
    // directly into our own queue.

    if (!m_workers.empty())
    {
        // Add round-robin style. Note that work-stealing helps load-balance,
        // if it hasn't been disabled. If it has we may need to consider a
        // smarter scheme here.
        state = m_workers[m_nextRoundRobinWorkerIndex]->PushJob(delegate);
        state->m_debugChar = debugChar;

        m_nextRoundRobinWorkerIndex = (m_nextRoundRobinWorkerIndex + 1) % m_workers.size();
    }

    return state;
}

void jobsystem::JobManager::AssistUntilJobDone(JobStatePtr state)
{
    JOBSYSTEM_ASSERT(state->m_ready.load(std::memory_order_acquire));

    const affinity_t workerAffinity = kAffinityAllBits;

    // Steal jobs from workers until the specified job is done.
    while (!state->IsDone())
    {
        JOBSYSTEM_ASSERT(!m_workers.empty());

        JobQueueEntry job;
        bool hasUnsatisfiedDependencies;

        if (m_workers[0]->PopNextJob(job, hasUnsatisfiedDependencies, true, workerAffinity))
        {
            Observer(job, EJobEvent::eJobEvent_JobStart, m_workers.size(), job.m_state->m_jobId);
            job.m_delegate(job.m_state->m_threadNr, job.m_state->m_threadCounts);
            Observer(job, EJobEvent::eJobEvent_JobDone, m_workers.size());

            job.m_state->SetDone();

            Observer(job, EJobEvent::eJobEvent_JobRunAssisted, 0);

            s_signalThreads.notify_one();
        }
    }
}

void jobsystem::JobManager::AssistUntilDone()
{
    JOBSYSTEM_ASSERT(!m_workers.empty());

    // Steal and run jobs from workers until all queues are exhausted.

    const affinity_t workerAffinity = kAffinityAllBits;

    JobQueueEntry job;
    bool foundBusyWorker = true;

    while (foundBusyWorker)
    {
        foundBusyWorker = false;

        for (JobSystemWorker* worker : m_workers)
        {
            if (worker->PopNextJob(job, foundBusyWorker, false, workerAffinity))
            {
                Observer(job, EJobEvent::eJobEvent_JobStart, m_workers.size(), job.m_state->m_jobId);
                job.m_delegate(job.m_state->m_threadNr, job.m_state->m_threadCounts);
                Observer(job, EJobEvent::eJobEvent_JobDone, m_workers.size());

                job.m_state->SetDone();

                Observer(job, EJobEvent::eJobEvent_JobRunAssisted, 0);

                foundBusyWorker = true;
                s_signalThreads.notify_one();
                break;
            }
        }
    }

    for (JobSystemWorker* worker : m_workers)
    {
        if (!worker->m_queue.empty())
        {
            JOBSYSTEM_ASSERT(0);
        }
    }
}

void jobsystem::JobManager::JoinWorkersAndShutdown(bool finishJobs)
{
    if (finishJobs)
    {
        AssistUntilDone();
    }

    // Tear down each worker. Un-popped jobs may still reside in the queues at this point
    // if finishJobs = false.
    // Don't destruct workers yet, in case someone's in the process of work-stealing.
    for (size_t i = 0, n = m_workers.size(); i < n; ++i)
    {
        JOBSYSTEM_ASSERT(m_workers[i]);
        m_workers[i]->Shutdown();
    }

    // Destruct all workers.
    std::for_each(m_workers.begin(), m_workers.end(), [](JobSystemWorker* worker) { delete worker; });
    m_workers.clear();

#ifdef JOBSYSTEM_ENABLE_PROFILING

    delete[] m_timelines;
    m_timelines = nullptr;

#endif // JOBSYSTEM_ENABLE_PROFILING
}

void jobsystem::JobManager::DumpProfilingResults()
{
#ifdef JOBSYSTEM_ENABLE_PROFILING

    AssistUntilDone();

    auto now = ProfileClockNow();
    auto totalNS = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_firstJobTime).count();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    const size_t workerCount = m_workers.size();

    printf(
        "\n[Job System Statistics]\n"
        "Jobs Run:       %8d\n" // May be < jobs submitted
        "Jobs Stolen:    %8d\n"
        "Jobs Assisted:  %8d\n"
        "Workers Used:   %8lu\n"
        "Workers Awoken: %8lu\n"
        ,
        m_jobsRun.load(std::memory_order_acquire),
        m_jobsStolen.load(std::memory_order_acquire),
        m_jobsAssisted.load(std::memory_order_acquire),
        CountBits(m_usedMask.load(std::memory_order_acquire)),
        CountBits(m_awokenMask.load(std::memory_order_acquire)));

    printf("\n[Worker Profiling Results]\n%.3f total ms\n\nTimeline (approximated):\n\n", double(totalNS) / 1000000);

    const char* busySymbols = "abcdefghijklmn";
    const size_t busySymbolCount = strlen(busySymbols);

    for (size_t workerIndex = 0; workerIndex < workerCount + 1; ++workerIndex)
    {
        ProfilingTimeline& timeline = m_timelines[workerIndex];

        const char* name = (workerIndex < workerCount) ? m_workers[workerIndex]->m_desc.m_name.c_str() : "[Assist]";

        const size_t bufferSize = 200;
        char buffer[bufferSize];
        snprintf(buffer, sizeof(buffer), "%20s: ", name);

        const size_t nameLen = strlen(buffer);
        const size_t remaining = bufferSize - nameLen - 2;

        for (size_t nameIdx = nameLen; nameIdx < bufferSize - 2; ++nameIdx)
        {
            buffer[nameIdx] = '-';
        }

        buffer[bufferSize - 2] = '\n';
        buffer[bufferSize - 1] = 0;

        for (ProfilingTimeline::TimelineEntry& entry : timeline.m_entries)
        {
            const auto startNs = std::chrono::duration_cast<std::chrono::nanoseconds>(entry.start - m_firstJobTime).count();
            const auto endNs = std::chrono::duration_cast<std::chrono::nanoseconds>(entry.end - m_firstJobTime).count();

            const double startPercent = (double(startNs) / double(totalNS));
            const double endPercent = (double(endNs) / double(totalNS));

            const char jobCharacter = (entry.debugChar != 0) ? entry.debugChar : busySymbols[entry.jobId % busySymbolCount];

            const size_t startIndex = nameLen + std::min<size_t>(remaining - 1, size_t(startPercent * double(remaining)));
            size_t endIndex = nameLen + std::min<size_t>(remaining - 1, size_t(endPercent * double(remaining)));

            size_t shift = 0;

            while (buffer[startIndex + shift] != '-' && startIndex + shift < bufferSize - 3 && endIndex + shift < bufferSize - 3)
            {
                ++shift;
            }

            endIndex -= std::min<size_t>(endIndex - startIndex, size_t(shift));

            for (size_t i = startIndex + shift; i <= endIndex + shift; ++i)
            {
                JOBSYSTEM_ASSERT(i < bufferSize - 2);
                buffer[i] = jobCharacter;
            }
        }

        printf("%s", buffer);
    }

    printf("\n");

#endif // JOBSYSTEM_ENABLE_PROFILING
}