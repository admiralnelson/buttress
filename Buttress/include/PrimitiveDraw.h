#pragma once
#include "pch.h"
//#include "Shader.h"
struct DrawCommand
{
	std::string Description;
	std::function<void()> Command;
};

class PrimitiveDraw
{
public:
	void PushBegin(DrawCommand cmd);
	void PushLoop(DrawCommand cmd);
	void PopBegin(DrawCommand cmd);
	void PopLoop(DrawCommand cmd);
	void Debug();
	DrawCommand GetBeginQueue(size_t idx);
	DrawCommand GetLoopQueue(size_t idx);
	size_t GetBeginQueueSize() { return m_beginQueue.size(); }
	size_t GetLoopQueueSize() { return m_loopQueue.size(); }
	DrawCommand GetCurrentBeginQueue();
	DrawCommand GetCurrentLoopQueue();
private:
	std::deque<DrawCommand> m_beginQueue;
	std::deque<DrawCommand> m_loopQueue;
	DrawCommand m_currentBeginQueue;
	DrawCommand m_currentLoopQueue;
	size_t currentIdxBegin = 0;
	size_t currentIdxLoop = 0;
};