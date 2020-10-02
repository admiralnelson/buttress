#include "pch.h"
#include "ECSThreading.h"

void ECSThreading::InitialiseThreads(unsigned int jobs, std::vector<std::function<void(unsigned int, unsigned int)>> tasksToDo, std::function<void()> taskOnComplete)
{
	if (m_isRunning)
	{
		PRINT("WARNING", "cannot assign jobs unless the slaves have been stopped!");
		return;
	}
	
	m_taskToDo = tasksToDo;
	m_totalWorkers = jobs;
	m_onfinish = taskOnComplete;
}

void ECSThreading::AddTask(std::function<void(unsigned int, unsigned int)> tasksToDo)
{
	m_taskToDo.push_back(tasksToDo);
}

bool ECSThreading::AreWorkersCompleted()
{
	return AllBitsAreTrue();
}

void ECSThreading::StartTheJobs()
{
	if (m_isRunning) return;
	m_isRunning = true;
	for (size_t i = 0; i < m_totalWorkers; i++)
	{
		m_threads.push_back(std::thread([&]()
		{
			unsigned int entitiesPerWorker = m_universe->GetTotalEntities() / m_totalWorkers;
			//for example there are 100 entities.
			//and there are 10 workers (100/10 = entitiesPerJob)
			//then every worker should work on 10 entities each
			// i starts at 0, so begin would be 0 * 10 + 0 = 0
			//                   end would be (0 * 10 + 0) + 10 = 10
			// i starts at 1, begin 1 * 10 + 1 = 11
			//                end  (1 * 10 + 1) + 10 = 21
			unsigned int entityBegin = i * entitiesPerWorker + i;
			unsigned int entityEnds = entityBegin + entitiesPerWorker;
			while (m_isRunning)
			{
				SetBit(i, false);
				//do the work
				for (const auto& t : m_taskToDo)
				{
					t(entityBegin, entityEnds);
				}
				//submit to master
				SetBit(i, true);
			}
		}));
	}
}

bool ECSThreading::AllBitsAreTrue()
{
	unsigned int mask = (1ULL << 1) - 1;
	return ((m_jobBits + 1) & mask) <= 1;
}

bool ECSThreading::CheckBit(unsigned int bitAtIndex)
{
	if (bitAtIndex >= m_totalWorkers) return false;
	return (!!((m_jobBits) & (1ULL << (bitAtIndex))));
}

void ECSThreading::SetBit(unsigned int bitAtIndex, bool val)
{
	if (bitAtIndex >= m_totalWorkers)
	{
		PRINT("ERROR", "JOB is out of range. max index is: ", m_totalWorkers - 1, "index to access was:", bitAtIndex);
		throw std::exception("job is out of range");
	}
	m_jobBits ^= (-val ^ m_jobBits) & (1UL << bitAtIndex);

}
