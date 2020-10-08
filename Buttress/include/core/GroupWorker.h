#pragma once
#include "pch.h"

typedef unsigned int ThreadNr;
typedef unsigned int NrOfThreads;

class GroupWorker
{
public:
	GroupWorker(unsigned int nrOfWorkers);
	void Start();
	void Stop();
	bool IsReady();
	void PushJob(std::function<void(ThreadNr, NrOfThreads)> task);
	void BlockUntilFinished();
	bool IsJobCompleted();
private:
	void DoWork(ThreadNr thread);

	std::deque<std::function<void(ThreadNr, NrOfThreads)>> m_tasks;
	std::vector<std::thread> m_workers;
	std::condition_variable m_cvTasks;
	std::condition_variable m_cvTasksDone;
	std::mutex m_queueMutex;
	NrOfThreads m_nrThreads;
	unsigned int m_busyCounter;
	std::atomic<unsigned int> m_processed;
	bool m_isReady;
	bool m_isRunning;
};