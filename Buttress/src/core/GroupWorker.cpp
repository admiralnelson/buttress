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
	m_cvTasksDone.wait(lock, [this]() { return  (m_busyCounter == 0); });
	
}

bool GroupWorker::IsJobCompleted()
{
	return m_tasks.empty() && (m_busyCounter == 0);
}

void GroupWorker::DoWork(ThreadNr threadNr)
{
	while (m_isRunning)
	{
		size_t len = m_tasks.size();
		for (size_t i = 0; i < len; i++)
		{
			m_busyCounter++;

			auto function = m_tasks[i]; //m_tasks.front();
			//m_tasks.pop_front();

			//lock.unlock();

			function(threadNr, m_nrThreads);
			m_processed++;

			//lock.lock();
			//m_cvTasksDone.notify_one();
			m_busyCounter--;
		}
	}
}
