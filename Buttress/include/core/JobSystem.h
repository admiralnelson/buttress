#pragma once

#define JOBSYSTEM_ASSERT(...) assert(__VA_ARGS__) ///< Directs internal system asserts to app-specific assert mechanism.

#include "pch.h"

namespace jobsystem
{
    typedef unsigned int ThreadNr;
    typedef unsigned int NrOfThreads;
    inline uint64_t GetBit(uint64_t n)
    {
        return static_cast<uint64_t>(1) << n;
    }

    inline size_t CountBits(uint64_t n)
    {
        size_t bits = 0;
        while (n)
        {
            bits += n & 1;
            n >>= 1;
        }
        return bits;
    }

    typedef std::function<void(ThreadNr, NrOfThreads)> JobDelegate;          ///< Structure of callbacks that can be requested as jobs.

    typedef uint64_t affinity_t;

    static const affinity_t kAffinityAllBits = static_cast<affinity_t>(~0);

    inline affinity_t CalculateSafeWorkerAffinity(size_t workerIndex, size_t workerCount)
    {
        affinity_t affinity = kAffinityAllBits; // Set all bits so jobs with affinities out of range can still be processed.
        affinity &= ~(workerCount - 1);         // Wipe bits within valid range.
        affinity |= GetBit(workerIndex);        // Set worker-specific bit.

        return affinity;
    }

    /**
     * Offers access to the state of job.
     * In particular, callers can use the Wait() function to ensure a given job is complete,
     * or Cancel().
     * Note, however, that doing so is not good practice with job systems. If no hardware threads
     * are available to process a given job, you can stall the caller for significant time.
     *
     * Internally, the state manages dependencies as well as atomics describing the status of the job.
     */
    typedef std::shared_ptr<class JobState> JobStatePtr;

    class JobState
    {
    private:

        friend class JobSystemWorker;
        friend class JobManager;

        std::atomic<bool>           m_cancel;           ///< Is the job pending cancellation?
        std::atomic<bool>           m_ready;            ///< Has the job been marked as ready for processing?

        std::vector<JobStatePtr>    m_dependants;       ///< List of dependent jobs.
        std::atomic<int>            m_dependencies;     ///< Number of outstanding dependencies.

        std::atomic<bool>           m_done;             ///< Has the job executed to completion?
        std::condition_variable     m_doneSignal;
        std::mutex                  m_doneMutex;

        affinity_t                  m_workerAffinity;   ///< Option to limit execution to specific worker threads / cores.

        size_t                      m_jobId;            ///< Debug/profiling ID.
        char                        m_debugChar;        ///< Debug character for profiling display.

        ThreadNr                    m_threadNr;
        NrOfThreads                 m_threadCounts;

        void SetQueued();

        void SetDone();

        bool AwaitingCancellation() const;

    public:

        JobState();

        ~JobState() {}

        JobState& SetReady();

        JobState& Cancel();

        JobState& AddDependant(JobStatePtr dependant);

        JobState& SetWorkerAffinity(affinity_t affinity);

        bool IsDone() const;

        bool Wait(size_t maxWaitMicroseconds = 0);

        bool AreDependenciesMet() const;

        bool HasDependencies() const;
    };

    /**
     * Represents an entry in a job queue.
     * - A delegate to invoke
     * - Internal job state
     */
    struct JobQueueEntry
    {
        JobDelegate     m_delegate;     ///< Delegate to invoke for the job.
        JobStatePtr     m_state;        ///< Pointer to job state.
    };

    /**
     * Descriptor for a given job worker thread, to be provided by the host application.
     */
    struct JobWorkerDescriptor
    {
        JobWorkerDescriptor(const char* name = "JobSystemWorker", affinity_t cpuAffinity = affinity_t(~0), bool enableWorkSteeling = true)
            : m_name(name)
            , m_cpuAffinity(cpuAffinity)
            , m_enableWorkStealing(enableWorkSteeling)
        {
        }

        std::string     m_name;                     ///< Worker name, for debug/profiling displays.
        affinity_t      m_cpuAffinity;              ///< Thread affinity. Defaults to all cores.
        bool            m_enableWorkStealing : 1;   ///< Enable queue-sharing between workers?
    };

    /**
     * Job events (for tracking/debugging).
     */
    enum class EJobEvent
    {
        eJobEvent_JobPopped,            ///< A job was popped from a queue.
        eJobEvent_JobStart,             ///< A job is about to start.
        eJobEvent_JobDone,              ///< A job just completed.
        eJobEvent_JobRun,               ///< A job has been completed.
        eJobEvent_JobRunAssisted,       ///< A job has been completed through outside assistance.
        eJobEvent_JobStolen,            ///< A worker has stolen a job from another worker. 
        eJobEvent_WorkerAwoken,         ///< A worker has been awoken.
        eJobEvent_WorkerUsed,           ///< A worker has been utilized.
    };

    typedef std::function<void(const JobQueueEntry & job, EJobEvent, uint64_t, size_t)> JobEventObserver;  ///< Delegate definition for job event observation.

    typedef std::deque<JobQueueEntry> JobQueue;     ///< Data structure to represent job queue.

    /**
     * High-res clock based on windows performance counter. Supports STL chrono interfaces.
     */
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    inline TimePoint ProfileClockNow()
    {
        return std::chrono::high_resolution_clock::now();
    }

    /**
     * Tracking each job's start/end times in a per-worker timeline, for debugging/profiling.
     */
    class ProfilingTimeline
    {
    public:

        struct TimelineEntry
        {
            uint64_t                        jobId;                  ///< ID of the job that generated this timeline entry.
            TimePoint                       start;                  ///< Job start time.
            TimePoint                       end;	                ///< Job end time.
            char                            debugChar;              ///< Job's debug character for profiling display.

            std::string                     description;            ///< Timeline entry description.
        };

        typedef std::vector<TimelineEntry> TimelineEntries;

        TimelineEntries                     m_entries;              //< List of timeline entries for this thread.
    };

    /**
     * Represents a worker thread.
     * - Owns a job queue
     * - Implements work-stealing from other workers
     */
    class JobSystemWorker
    {
        friend class JobManager;

    public:

        JobSystemWorker(const JobWorkerDescriptor& desc, const JobEventObserver& eventObserver)
            : m_allWorkers(nullptr)
            , m_workerCount(0)
            , m_workerIndex(0)
            , m_desc(desc)
            , m_eventObserver(eventObserver)
            , m_hasShutDown(false)
        {
        }

        void Start(size_t index, JobSystemWorker** allWorkers, size_t workerCount);

        void Shutdown();

        JobStatePtr PushJob(JobDelegate delegate);

    private:

        void NotifyEventObserver(const JobQueueEntry& job, EJobEvent event, uint64_t workerIndex, size_t jobId = 0);

        bool PopJobFromQueue(JobQueue& queue, JobQueueEntry& job, bool& hasUnsatisfiedDependencies, affinity_t workerAffinity);

        bool PopNextJob(JobQueueEntry& job, bool& hasUnsatisfiedDependencies, bool useWorkStealing, affinity_t workerAffinity);

        void SetThreadName(const char* name);

        void WorkerThreadProc();

        std::thread                 m_thread;                   ///< Thread instance for worker.
        std::atomic<bool>           m_stop;                     ///< Has a stop been requested?
        std::atomic<bool>           m_hasShutDown;              ///< Has the worker completed shutting down?

        mutable std::mutex          m_queueLock;                ///< Mutex to guard worker queue.
        JobQueue                    m_queue;                    ///< Queue containing requested jobs.

        JobSystemWorker** m_allWorkers;               ///< Pointer to array of all workers, for queue-sharing / work-stealing.
        NrOfThreads                 m_workerCount;              ///< Number of total workers (size of m_allWorkers array).
        ThreadNr                    m_workerIndex;              ///< This worker's index within m_allWorkers.

        JobEventObserver            m_eventObserver;            ///< Observer of job-related events occurring on this worker.
        JobWorkerDescriptor         m_desc;                     ///< Descriptor/configuration of this worker.
    };

    /**
     * Descriptor for configuring the job manager.
     * - Contains descriptor for each worker
     */
    struct JobManagerDescriptor
    {
        std::vector<JobWorkerDescriptor> m_workers;             ///< Configurations for all workers that should be spawned by JobManager.
    };

    /**
     * Manages job workers, and acts as the primary interface to the job queue.
     */
    class JobManager
    {
    private:

        void Observer(const JobQueueEntry& job, EJobEvent event, uint64_t workerIndex, size_t jobId = 0);

    public:

        JobManager()
            : m_jobsRun(0)
            , m_jobsStolen(0)
            , m_usedMask(0)
            , m_awokenMask(0)
            , m_nextRoundRobinWorkerIndex(0)
            , m_timelines(nullptr)
            , m_firstJobTime()
        {

        }

        ~JobManager();

        bool Create(const JobManagerDescriptor& desc);

        JobStatePtr AddJob(JobDelegate delegate, char debugChar = 0);

        void AssistUntilJobDone(JobStatePtr state);

        void AssistUntilDone();

        void JoinWorkersAndShutdown(bool finishJobs = false);

    private:

        size_t                          m_nextRoundRobinWorkerIndex;    ///< Index of the worker to receive the next requested job, round-robin style.

        std::atomic<unsigned int>       m_jobsRun;                      ///< Counter to track # of jobs run.
        std::atomic<unsigned int>       m_jobsAssisted;                 ///< Counter to track # of jobs run via external Assist*().
        std::atomic<unsigned int>       m_jobsStolen;                   ///< Counter to track # of jobs stolen from another worker's queue.
        std::atomic<affinity_t>         m_usedMask;                     ///< Mask with bits set according to the IDs of the jobs that have executed jobs.
        std::atomic<affinity_t>         m_awokenMask;                   ///< Mask with bits set according to the IDs of the jobs that have been awoken at least once.

    private:

        JobManagerDescriptor             m_desc;                         ///< Descriptor/configuration of the job manager.

        bool                            m_hasPushedJob;                 ///< For profiling - has a job been pushed yet?
        TimePoint                       m_firstJobTime;                 ///< For profiling - when was the first job pushed?
        ProfilingTimeline* m_timelines;                    ///< For profiling - a ProfilingTimeline entry for each worker, plus an additional entry to represent the Assist thread.

        std::vector<JobSystemWorker*>   m_workers;                      ///< Storage for worker instances.

        void DumpProfilingResults();
    };

    /**
     * Helper for building complex job/dependency chains logically.
     *
     * e.g.
     *
     * jobsystem::JobManager jobManager;
     * ...
     * jobsystem::JobChainBuilder<128>(jobManager)
     *  .Do(something, 'a')
     *  .Then()
     *  .Do(somethingAfterThat, 'b')
     *  .Then()
     *  .Together()
     *      .Do(parallelThing1, 'c')
     *      .Do(parallelThing2, 'd')
     *      .Do(parallelThing3, 'e')
     *  .Close()
     *  .Then()
     *  .Do(finalThing, 'F')
     *  .Go()
     * .WaitForAll();
     **                                    --- parallelThing1 ---
     *                                   /                       \
     *  something -> somethingAfterThat -> --- parallelThing2 ---- -> finalThing
     *                                   \                       /
     *                                     --- parallelThing3 ---
     * etc...
     *
     */
    template<size_t MaxJobNodes = 256>
    class JobChainBuilder
    {
    public:

        struct Node
        {
            Node() : isGroup(false), groupDependency(nullptr) {}
            ~Node() {}

            Node* groupDependency;
            JobStatePtr		job;
            bool			isGroup;
        };

        Node* AllocNode()
        {
            if (m_nextNodeIndex >= MaxJobNodes)
                return nullptr;

            Node* node = &m_nodePool[m_nextNodeIndex++];
            *node = Node();

            return node;
        }

        JobChainBuilder(JobManager& manager)
            : mgr(manager)
        {
            Reset();

            // Push a sentinel (root) node.
            m_stack.push_back(AllocNode());
        }

        void Reset()
        {
            m_allJobs.clear();
            m_stack.clear();

            m_last = nullptr;
            m_dependency = nullptr;
            m_nextNodeIndex = 0;
            m_failed = false;
        }

        JobChainBuilder& Together(char debugChar = 0)
        {
            if (Node* item = AllocNode())
            {
                item->isGroup = true;
                item->groupDependency = m_dependency;

                JobDelegate dummy = [](ThreadNr x, NrOfThreads y) {x; y; };
                item->job = mgr.AddJob(dummy, debugChar);

                m_allJobs.push_back(item->job);

                m_last = item;
                m_dependency = nullptr;

                m_stack.push_back(item);
            }
            else
            {
                Fail();
            }

            return *this;
        }

        JobChainBuilder& Do(JobDelegate delegate, char debugChar = 0)
        {
            Node* owner = m_stack.back();

            if (Node* item = AllocNode())
            {
                item->job = mgr.AddJob(delegate, debugChar);

                m_allJobs.push_back(item->job);

                if (m_dependency)
                {
                    m_dependency->job->AddDependant(item->job);
                    m_dependency = nullptr;
                }

                if (owner && owner->isGroup)
                {
                    item->job->AddDependant(owner->job);

                    if (owner->groupDependency)
                    {
                        owner->groupDependency->job->AddDependant(item->job);
                    }
                }

                m_last = item;
            }
            else
            {
                Fail();
            }

            return *this;
        }

        JobChainBuilder& Then()
        {
            m_dependency = m_last;
            m_last = (m_dependency) ? m_dependency->groupDependency : nullptr;

            return *this;
        }

        JobChainBuilder& Close()
        {
            if (!m_stack.empty())
            {
                Node* owner = m_stack.back();
                if (owner->isGroup)
                {
                    m_last = owner;
                }
            }

            m_dependency = nullptr;

            if (m_stack.size() > 1)
            {
                m_stack.pop_back();
            }

            return *this;
        }

        JobChainBuilder& Go()
        {
            if (m_allJobs.empty())
            {
                return *this;
            }

            Then();
            JobDelegate dummy = [](ThreadNr x, NrOfThreads y) {x; y; };
            Do(dummy, 'J');
            m_joinJob = m_allJobs.back();

            for (JobStatePtr& job : m_allJobs)
            {
                job->SetReady();
            }

            return *this;
        }

        void Fail()
        {
            for (JobStatePtr& job : m_allJobs)
            {
                job->Cancel();
            }

            m_allJobs.clear();
            m_failed = true;
        }

        bool Failed() const
        {
            return m_failed;
        }

        void WaitForAll()
        {
            if (m_joinJob)
            {
                m_joinJob->Wait();
            }
        }

        void AssistAndWaitForAll()
        {
            mgr.AssistUntilJobDone(m_joinJob);
        }

        JobManager& mgr;                        ///< Job manager to submit jobs to.

        Node                        m_nodePool[MaxJobNodes];    ///< Pool of chain nodes (on the stack). The only necessary output of this system is jobs. Nodes are purely internal.
        size_t                      m_nextNodeIndex;            ///< Next free item in the pool.

        std::vector<Node*>          m_stack;                    ///< Internal stack to track groupings.
        std::vector<JobStatePtr>    m_allJobs;                  ///< All jobs created by the builder, to be readied on completion.

        Node* m_last;                     ///< Last job to be pushed, to handle setting up dependencies after Then() calls.
        Node* m_dependency;               ///< Any job promoted to a dependency for the next job, as dicated by Then().

        JobStatePtr                 m_joinJob;                  ///< Final join job that callers can wait on to complete the batch.

        bool                        m_failed;                   ///< Did an error occur during creation of the DAG?
    };


} // namespace jobsystem
