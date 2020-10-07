#include "pch.h"
#include "core/GroupWorker.h"

GroupWorker::GroupWorker(unsigned int nrOfWorkers)
{
	m_nrThreads = nrOfWorkers;
}

void GroupWorker::Start()
{
	if (m_isRunning)
	{
		PRINT("WARNING", "group worker is already started!");
		return;
	}
	m_isRunning = true;
	for (ThreadNr i = 0; i < m_nrThreads; i++)
	{
		m_workers.emplace_back([&, i] { DoWork(i); });
	}
}

void GroupWorker::PushJob(std::function<void(ThreadNr, NrOfThreads)> task)
{
	std::unique_lock<std::mutex> lock(m_queueMutex);
	m_tasks.emplace_back(task);
	m_cvTasks.notify_one();
}

void GroupWorker::BlockUntilFinished()
{
	if (!m_isRunning)
	{
		PRINT("ERROR", "DEADLOCK!, group worker is never started");
		throw std::runtime_error("GROUP WORKER IS NOT STARTED!");
	}
	std::unique_lock<std::mutex> lock(m_queueMutex);
	m_cvTasksDone.wait(lock, [this]() { return m_tasks.empty() && (m_busyCounter == 0); });
}

void GroupWorker::DoWork(ThreadNr threadNr)
{
	while (m_isRunning)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_cvTasks.wait(lock, [this]() { return !m_isRunning || !m_tasks.empty(); });
		if (!m_tasks.empty())
		{
			m_busyCounter++;

			auto function = m_tasks.front();
			m_tasks.pop_front();

			lock.unlock();

			function(threadNr, m_nrThreads);
			m_processed++;

			lock.lock();
			m_busyCounter--;
			m_cvTasksDone.notify_one();
		}
	}
}
