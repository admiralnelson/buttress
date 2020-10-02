#pragma once
#include "pch.h"
#include "ECS.h"
class ECSThreading
{
	friend class Universe;
public:
	void InitialiseThreads(unsigned int jobs, std::vector<std::function<void(unsigned int, unsigned int)>> tasksToDo, std::function<void()> taskOnComplete);
	void AddTask(std::function<void(unsigned int, unsigned int)> tasksToDo);
	bool AreWorkersCompleted();
private:
	void StartTheJobs();
	void StopTheJobs();
	bool AllBitsAreTrue();
	bool CheckBit(unsigned int bitAtIndex);
	void SetBit(unsigned int bitAtIndex, bool val);
	
	bool m_isRunning = false;
	unsigned int m_totalWorkers = 0;
	std::atomic<unsigned int> m_jobBits = 0;
	std::vector<std::thread> m_threads;
	std::vector<std::function<void(unsigned int, unsigned int)>> m_taskToDo;
	std::function<void()> m_onfinish;
	Universe* m_universe;

};