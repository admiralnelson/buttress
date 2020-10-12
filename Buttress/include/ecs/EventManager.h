#pragma once
#include "pch.h"

namespace WINDOW_EVENT
{
	const EventId WINDOW_RESIZE = 0;
	namespace PARAMS
	{
		const EventId PARAM_WIDTH = 1;
		const EventId PARAM_HEIGHT = 2;
	}
}
namespace MOUSE_INPUT_EVENT
{
	const EventId MOUSE_MOVE = 3;
	namespace PARAMS
	{
		const EventId MOUSE_X = 4;
		const EventId MOUSE_Y = 5;
	}
}
namespace KEYBOARD_EVENT
{
	const EventId KEYBOARD_PRESS = 6;
	namespace PARAMS
	{
		const EventId KEYBOARD_BUTTON = 7;
	}
}

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