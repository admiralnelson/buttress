#pragma once
#include "pch.h"

typedef unsigned int EventId;

namespace WINDOW_EVENT
{
	const EventId WINDOW_RESIZE = 0;
	namespace PARAMS
	{
		const EventId PARAM_WIDTH = 1;
		const EventId PARAM_HEIGHT = 2;
	}
}
const EventId MOUSE_MOVEMENT = 3;
const EventId KEYBOARD_MOVEMENT = 4;

class Event
{
public: 
	Event() = delete;
	explicit Event(EventId type) : m_type(type) {};
	template <typename EVENT_PARAM_TYPE>
	void SetParam(EventId id, EVENT_PARAM_TYPE value)
	{
		m_data[id] = value;
	}

	template <typename EVENT_PARAM_TYPE>
	EVENT_PARAM_TYPE GetParam(EventId id)
	{
		return std::any_cast<EVENT_PARAM_TYPE>(m_data[id]);
	}


	EventId GetType()
	{
		return m_type;
	}

private:
	const EventId m_type;
	std::unordered_map<EventId, std::any> m_data;
};


class EventManager
{
public:
	void AddEvent(EventId eventId, std::function<void(Event&)> callback);
	void SendEvent(Event& evt);
	void SendEvent(EventId evt);
private:
	std::unordered_map<EventId, std::list<std::function<void(Event&)>>> m_eventListeners;
};