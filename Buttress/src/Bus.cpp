#include "pch.h"
#include "Bus.h"

void Bus::Start()
{
	if (!m_started)
	{
		PRINT("INFO","Bus is started");
		m_started = true;
		m_thread = std::thread(&Bus::ThreadTick, this);
		m_thread.detach();
	}
}

void Bus::Stop()
{
	m_started = false;
	for (auto& i : m_nodes)
	{
		delete i;
	}
	if (!m_messages.empty())
	{
			m_messages.erase(m_messages.begin() + m_messages.size() - 1);
	}
	m_nodes.clear();
	if (!m_started) return;
}

void Bus::AddReceiver(std::string name, std::string triggerOnTag, std::function<void(Message&)> onReceive)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	if (IsSubscriberExist(name))
	{
		PRINT("ERROR", "subscriber", name, "is in the bus!");
		return;
	}
	Node* node = new Node{ name,  triggerOnTag, onReceive };
	m_nodes.push_back(node);
}

void Bus::RemoveReceiver(std::string name)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	size_t i = 0;
	for (i = 0; i < m_nodes.size(); i++)
	{
		if (m_nodes[i]->name == name)
		{
			delete m_nodes[i];
			break;
		}
	}
	if (i == m_nodes.size() - 1) return;
	m_nodes.erase(m_nodes.begin() + i);
}

void Bus::SendMessage(Message message)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	m_messages.push_back(message);
}

void Bus::Tick()
{
	while (!m_messages.empty())
	{
		for (auto& i : m_nodes)
		{
			if (i->triggerOnTag != m_messages.front().tag) continue;
			if (i->onReceive)
			{
				Message& msg = m_messages.front();
				if (spoolToConsole)
				{
					PRINT("EVENT", "tag:", msg.tag, "message:", msg.msg);
				}
				i->onReceive(msg);
			}
		}
		m_messages.pop_front();
	}
}

void Bus::ThreadTick()
{
	while (m_started)
	{
		Tick();
	}
}

void Bus::Debug()
{
	PRINT("INFO", "subscriber count", m_nodes.size());
	for (auto& i : m_nodes)
	{
		auto* x = &i->onReceive;
		PRINT("  - ", x, i->name);
	}
}

bool Bus::IsSubscriberExist(std::string name)
{
	for (auto& i : m_nodes)
	{
		if (i->name == name) return true;
	}
	return false;
}

Bus::~Bus()
{
	Stop();
}
