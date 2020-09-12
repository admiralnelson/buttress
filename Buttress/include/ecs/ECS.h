#pragma once
#include "pch.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "system/EntityNameCheckSystem.h"

class Entity;
class EntityNameCheckSystem;
struct EntityName;
class Universe
{
	friend class Entity;
public:
	Entity CreateEntity(std::string name);
	Entity QueryByEntityId(EntityId id);
	Entity QueryByName(std::string name);

	Universe();
	~Universe()
	{
		PRINT("INFO", "the end times has come... (universe destroyed)", this);
	}
	void MemoryDebug()
	{
		PRINT("MEMORY DIAGNOSTIC");
		m_entityManager->MemoryDebug();
		m_componentManager->MemoryDebug();
		m_systemManager->MemoryDebug();
	}
private:
	std::unique_ptr<ComponentManager> m_componentManager = std::make_unique<ComponentManager>();
	std::unique_ptr<EntityManager> m_entityManager       = std::make_unique<EntityManager>();
	std::unique_ptr<SystemManager> m_systemManager       = std::make_unique<SystemManager>();
	std::shared_ptr<EntityNameCheckSystem> m_nameCheckerSystem;
};
class Entity
{
	friend class Universe;
public:
	Entity(Entity& ent): id(ent.id), m_universe(ent.m_universe)
	{
		
	}

	template<typename COMPONENT_TYPE>
	void AddComponent(COMPONENT_TYPE component)
	{
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::exception("invalid entity");
		}
		m_universe->m_componentManager->AddComponent<COMPONENT_TYPE>(id, component);

		auto signature = m_universe->m_entityManager->GetSignature(id);
		signature.set(m_universe->m_componentManager->GetComponentType<COMPONENT_TYPE>(), true);
		m_universe->m_entityManager->SetSignature(id, signature);

		m_universe->m_systemManager->EntitySignatureChanged(id, signature);
	}

	template<typename COMPONENT_TYPE>
	void RemoveComponent()
	{
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::exception("invalid entity");
		}
		if (strcmp(typeid(COMPONENT_TYPE).name(), typeid(EntityName).name()) == 0)
		{
			PRINT("ERROR", "attempt to remove core component (EntityName) from entity! entity id:", id);
			throw std::exception("attempt to remove core system from entity");
		}
		m_universe->m_componentManager->RemoveComponent<COMPONENT_TYPE>(id);

		auto signature = m_universe->m_entityManager->GetSignature(id);
		signature.set(m_universe->m_componentManager->GetComponentType<COMPONENT_TYPE>(), false);
		m_universe->m_entityManager->SetSignature(id, signature);

		m_universe->m_systemManager->EntitySignatureChanged(id, signature);
	}

	template<typename COMPONENT_TYPE>
	COMPONENT_TYPE& GetComponent()
	{
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::exception("invalid entity");
		}
		return m_universe->m_componentManager->GetComponent<COMPONENT_TYPE>(id);
	}

	template<typename SYSTEM_TYPE>
	void AttachToSystem()
	{
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::exception("invalid entity");
		}
		ComponentSignature objectCompSig = m_universe->m_entityManager->GetSignature();
		objectCompSig = objectCompSig | m_universe->m_systemManager->GetSignature<SYSTEM_TYPE>();
		m_universe->m_entityManager->SetSignature(objectCompSig);
	}

	template<typename SYSTEM_TYPE>
	void RemoveFromSystem()
	{
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::exception("invalid entity");
		}
		if (strcmp(typeid(SYSTEM_TYPE).name(), typeid(EntityNameCheckSystem).name()) == 0)
		{
			PRINT("ERROR", "attempt to remove core system (EntityNameCheckSystem) from entity! entity id:", id);
			throw std::exception("attempt to remove core system from entity");
		}
		ComponentSignature objectCompSig = m_universe->m_entityManager->GetSignature();
		objectCompSig = objectCompSig & m_universe->m_systemManager->GetSignature<SYSTEM_TYPE>();
		m_universe->m_entityManager->SetSignature(objectCompSig);
	}

	~Entity()
	{
		//don't. Let the user manually destroy their entity.
		//m_universe->m_entityManager->DestroyEntity(id);
	}
private:
	Entity(Universe* universe, EntityId _id) : m_universe(universe), id(_id) {}
	const EntityId id = 0;
	const Universe* m_universe = nullptr;
};

