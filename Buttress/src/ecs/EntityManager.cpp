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
	
	if (m_entitiesCount >= MAX_ENTITIES)
	{
		PRINT("ERROR", "TOO MANY ENTITIES. PAST THE LIMIT OF", MAX_ENTITIES);
		throw std::runtime_error("too many entities");
	}
	EntityId id;
	m_entities.try_pop(id);
	m_entitiesCount++;
	PRINT("created entity id:", id);
	return id;
}

void EntityManager::DestroyEntity(EntityId entity)
{
	PRINT("destroyed entity id:", entity );
	if (entity < 0)
	{
		PRINT("ERROR", "TOO ENTITIY OUT OF RANGE. entity was:", entity);
		throw std::runtime_error("entity out of range");
	}
	m_signatures[entity].reset();
	m_entities.push(entity);
	m_entitiesCount--;
}

void EntityManager::SetSignature(EntityId entity, ComponentSignature signature )
{
	
	if (entity < 0 && entity >= MAX_ENTITIES)
	{
		PRINT("ERROR", "TOO ENTITIY OUT OF RANGE. entity was:", entity);
		throw std::runtime_error("entity out of range");
	}
	m_signatures[entity] = signature;
}

unsigned int EntityManager::GetTotalEntities()
{
	return m_entitiesCount;
}

ComponentSignature EntityManager::GetSignature(EntityId entity)
{
	
	if (entity < 0 && entity >= MAX_ENTITIES)
	{
		PRINT("ERROR", "TOO ENTITIY OUT OF RANGE. entity was:", entity);
		throw std::runtime_error("entity out of range");
	}
	return m_signatures[entity];
}
