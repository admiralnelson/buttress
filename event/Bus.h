#pragma once
#include "pch.h"

struct Event 
{
	virtual ~Event() {};
	virtual std::string ToString() { return "an event"; };
};

enum class KeyboardButtonState
{
	RELEASE, TAP, HOLD
};

struct InputEvent
{
	double x;
	double y;
	int lastKey;
	float deltaT;
};

struct WindowEvent
{
	int x;
	int y;
	int width;
	int height;
	bool focus = true;
};

struct Message
{
	std::string tag;
	std::string msg;
	std::shared_ptr<Event> event;
	InputEvent inputEvent;
	WindowEvent windowEvent;
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
	void Start();
	void Stop();
	void AddReceiver(std::string name, std::string triggerOnTag, std::function<void(Message&)> onReceive);
	void RemoveReceiver(std::string name);
	void SendMessage(Message message);
	void Debug();
	void SetDeltaTime(float dt) { m_dt = dt; }
	float GetDeltaTime() { return m_dt; }
	bool IsSubscriberExist(std::string name);
	bool spoolToConsole = true;
	~Bus();
private:
	Bus() {};
	void Tick();
	void ThreadTick();
	struct Node 
	{
		std::string name;
		std::string triggerOnTag;
		std::function<void(Message&)> onReceive;
		//~Node() { PRINT("cleared"); }
	};
	std::vector<Node> m_nodes;
	std::deque<Message> m_messages;
	std::mutex m_mutex;
	std::thread m_thread;
	bool m_started = false;
	float m_dt = 0;
};