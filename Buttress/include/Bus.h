#pragma once
#include "pch.h"

struct Event 
{
	virtual ~Event() {};
	virtual std::string ToString() { return "an event"; };
};

struct Message
{
	std::string tag;
	std::string msg;
	Event *event;
};

class Bus
{
friend class Buttress;
public:	
	static Bus &Instance()
	{
		static Bus instance;
		return instance;
	}
	void AddReceiver(std::string name, std::function<void(Message&)> onReceive);
	void RemoveReceiver(std::string name);
	void SendMessage(Message& message);
	void Debug();
	bool IsSubscriberExist(std::string name);
	bool spoolToConsole = true;
	~Bus();
private:
	Bus() {};
	void Tick();
	struct Node 
	{
		std::string name;
		std::function<void(Message&)> onReceive;
	};
	std::vector<Node*> m_nodes;
	std::deque<Message> m_messages;
	std::mutex m_mutex;
};