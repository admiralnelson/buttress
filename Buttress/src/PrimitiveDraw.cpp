#include "pch.h"
#include "..\include\PrimitiveDraw.h"

void PrimitiveDraw::PushBegin(DrawCommand cmd)
{
	m_beginQueue.push_back(cmd);
}

void PrimitiveDraw::PushLoop(DrawCommand cmd)
{
	m_loopQueue.push_back(cmd);
}

void PrimitiveDraw::PopBegin(DrawCommand cmd)
{
	m_beginQueue.pop_front();
}

void PrimitiveDraw::PopLoop(DrawCommand cmd)
{
	m_loopQueue.pop_front();
}

void PrimitiveDraw::Debug()
{
	PRINT("INFO", "begin queue", ":");
	for (DrawCommand& c : m_beginQueue)
	{
		std::function<void()>* x = &c.Command;
		PRINT("-", x, " ", c.Description);
	}
	PRINT("INFO", "loop queue", ":");
	for (DrawCommand& c : m_loopQueue)
	{
		std::function<void()> *x = &c.Command;
		PRINT("-", x, " ", c.Description);
	}
}

DrawCommand PrimitiveDraw::GetBeginQueue(size_t idx)
{
	return m_beginQueue[idx];
}

DrawCommand PrimitiveDraw::GetLoopQueue(size_t idx)
{
	return m_loopQueue[idx];
}

DrawCommand PrimitiveDraw::GetCurrentBeginQueue()
{
	if (currentIdxBegin >= m_beginQueue.size())
	{
		currentIdxBegin = 0;
	}
	DrawCommand &d = m_beginQueue[currentIdxBegin];
	currentIdxBegin++;
	return d;
}

DrawCommand PrimitiveDraw::GetCurrentLoopQueue()
{
	if (currentIdxLoop >= m_loopQueue.size())
	{
		currentIdxLoop = 0;
	}
	DrawCommand &d =  m_loopQueue[currentIdxLoop];
	currentIdxLoop++;
	return d;
}
