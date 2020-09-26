#include "pch.h"
#include "ecs/EntityManager.h"

EntityManager::EntityManager()
{
	for (EntityId i = 0; i < MAX_ENTITIES; i++)
	{
		m_entities.push(i);
	}
	m_signatures.resize(MAX_ENTITIES);
}

EntityId EntityManager::CreateEntity()
{
	std::lock_guard mutex(m_mutex);
	if (m_entitiesCount >= MAX_ENTITIES)
	{
		PRINT("ERROR", "TOO MANY ENTITIES. PAST THE LIMIT OF", MAX_ENTITIES);
		throw std::exception("too many entities");
	}
	EntityId id = m_entities.front();
	m_entities.pop();
	m_entitiesCount++;
	PRINT("created entity id:", id);
	return id;
}

void EntityManager::DestroyEntity(EntityId entity)
{
	std::lock_guard mutex(m_mutex);
	PRINT("destroyed entity id:", entity );
	if (entity < 0)
	{
		PRINT("ERROR", "TOO ENTITIY OUT OF RANGE. entity was:", entity);
		throw std::exception("entity out of range");
	}
	m_signatures[entity].reset();
	m_entities.push(entity);
	m_entitiesCount--;
}

void EntityManager::SetSignature(EntityId entity, ComponentSignature signature )
{
	std::lock_guard mutex(m_mutex);
	if (entity < 0 && entity >= MAX_ENTITIES)
	{
		PRINT("ERROR", "TOO ENTITIY OUT OF RANGE. entity was:", entity);
		throw std::exception("entity out of range");
	}
	m_signatures[entity] = signature;
}

ComponentSignature EntityManager::GetSignature(EntityId entity)
{
	std::lock_guard mutex(m_mutex);
	if (entity < 0 && entity >= MAX_ENTITIES)
	{
		PRINT("ERROR", "TOO ENTITIY OUT OF RANGE. entity was:", entity);
		throw std::exception("entity out of range");
	}
	return m_signatures[entity];
}
