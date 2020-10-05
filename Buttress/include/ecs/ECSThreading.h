#pragma once
#include "pch.h"
class Universe;
class ECSThreading
{
	friend class Universe;
public:
	ECSThreading(Universe* univ) : m_universe(univ) {}
	void InitialiseThreads(unsigned int jobs, std::vector<std::function<void(unsigned int, unsigned int)>> tasksToDo, std::function<void()> taskOnComplete);
	void AddTask(std::function<void(unsigned int, unsigned int)> tasksToDo);
	bool AreWorkersCompleted();
	void Pause(bool yes=true);
	void Resume();
	bool IsPaused() { return m_paused; }
private:
	void StartTheJobs();
	void StopTheJobs();
	bool AllBitsAreTrue();
	bool AllBitsAreFalse();
	bool CheckBit(unsigned int bitAtIndex);
	void SetBit(unsigned int bitAtIndex, bool val);
	
	bool m_isRunning = false;
	unsigned int m_totalWorkers = 0;
	std::atomic<unsigned int> m_jobBits = 0;
	std::atomic<bool> m_paused = false;
	std::vector<std::thread> m_threads;
	std::vector<std::function<void(unsigned int, unsigned int)>> m_taskToDo;
	std::function<void()> m_onfinish;
	Universe* m_universe;

};