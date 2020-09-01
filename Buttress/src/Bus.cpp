#include "pch.h"
#include "Bus.h"

void Bus::AddReceiver(std::string name, std::function<void(Message&)> onReceive)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	if (IsSubscriberExist(name))
	{
		PRINT("ERROR", "subscriber", name, "is in the bus!");
		return;
	}
	Node* node = new Node{ name, onReceive };
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

void Bus::SendMessage(Message& message)
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
}
