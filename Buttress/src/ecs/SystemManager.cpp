#pragma once
#include "pch.h"
#include "SystemManager.h"


void SystemManager::EntityDestroyed(EntityId entity)
{
	for (auto& i : m_systems)
	{
		auto& system = i.second;
		system->m_entity.erase(entity);
	}
}

void SystemManager::EntitySignatureChanged(EntityId entity, ComponentSignature entitySignature)
{
	for (auto& i : m_systems)
	{
		auto& type = i.first;
		auto& system = i.second;
		auto& systemSignature = m_signatures[type];

		if ((entitySignature & systemSignature) == systemSignature)
		{
			system->m_entity.insert(entity);
		}
		else
		{
			system->m_entity.erase(entity);
		}
	}
}
