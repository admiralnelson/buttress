#include "pch.h"
#include "EventManager.h"

void EventManager::AddEvent(EventId eventId, std::function<void(Event&)> callback)
{
	m_eventListeners[eventId].push_back(callback);
}

void EventManager::SendEvent(Event& evt)
{
	EventId type = evt.GetType();
	for (const auto& listener : m_eventListeners[type])
	{
		listener(evt);
	}
}

void EventManager::SendEvent(EventId evt)
{
	Event event(evt);
	for (const auto& listener : m_eventListeners[evt])
	{
		listener(event);
	}
}
