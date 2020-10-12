#pragma once
#include "pch.h"
#include "SystemManager.h"


std::string SystemManager::GetAttachedSystemNames(EntityId entity, ComponentSignature componentSignature)
{
	/*std::vector<std::string> out;
	for (auto& i : m_systems)
	{
		auto& type = i.first;
		auto& system = i.second;
		auto& systemSignature = m_signatures[type];

		if (system->m_entity.find(entity) != system->m_entity.end())
		{
			return m_signaturesToName[componentSignature];
		}

	}
	return "";*/
	return "";
}

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
