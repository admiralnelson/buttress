#pragma once
#include "pch.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "EventManager.h"
#include "system/EntityNameCheckSystem.h"
#include "components/EntityName.h"
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
	
	void Render(float dt);

	template<typename COMPONENT_TYPE>
	void RegisterComponent()
	{
		m_componentManager->RegisterComponent<COMPONENT_TYPE>();
	}

	template<typename COMPONENT_TYPE>
	ComponentTypeId GetComponentType()
	{
		return m_componentManager->GetComponentType<COMPONENT_TYPE>();
	}

	template<typename SYSTEM_TYPE>
	ComponentSignature GetRequiredComponentForSystem()
	{
		return m_systemManager->GetSignature<SYSTEM_TYPE>();
	}

	template<typename SYSTEM_TYPE>
	SYSTEM_TYPE* GetSystem()
	{
		return m_systemManager->GetSystem<SYSTEM_TYPE>();
	}

	template<typename SYSTEM_TYPE>
	void RegisterSystem(ComponentSignature requiredComponents)
	{
		m_systemManager->RegisterSystem<SYSTEM_TYPE>(this);
		m_systemManager->SetSignature<SYSTEM_TYPE>(requiredComponents);
	}

	void AddEventListener(EventId evtId, std::function<void(Event&)> listener)
	{
		m_eventManager->AddEvent(evtId, listener);
	}

	void SendEvent(Event evt)
	{
		m_eventManager->SendEvent(evt);
	}

	void SendEvent(EventId evtId)
	{
		m_eventManager->SendEvent(evtId);
	}

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
	std::unique_ptr<EventManager> m_eventManager = std::make_unique<EventManager>();
	std::mutex m_mutex;
	float m_lastDt;
};
class Entity
{
	friend class Universe;
public:
	Entity() : id(INVALID_ENTITY), m_universe(nullptr)
	{

	}

	Entity(Entity& ent): id(ent.id), m_universe(ent.m_universe)
	{
		
	}

	friend bool operator==(const Entity& a, const Entity& b)
	{
		return a.id == b.id && a.m_universe == b.m_universe;
	}

	Entity operator=(const Entity& otherEntity)
	{
		Entity ent;
		ent.id = otherEntity.id;
		ent.m_universe = otherEntity.m_universe;
		return ent;
	}


	bool IsValid()
	{
		return id != INVALID_ENTITY && m_universe != nullptr;
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
		ComponentSignature objectCompSig = m_universe->m_entityManager->GetSignature(id);
		objectCompSig = objectCompSig | m_universe->m_systemManager->GetSignature<SYSTEM_TYPE>();
		m_universe->m_entityManager->SetSignature(id, objectCompSig);
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
		ComponentSignature objectCompSig = m_universe->m_entityManager->GetSignature(id);
		objectCompSig = objectCompSig & m_universe->m_systemManager->GetSignature<SYSTEM_TYPE>();
		m_universe->m_entityManager->SetSignature(id, objectCompSig);
	}

	/// O(N*2) operation!
	void Debug()
	{
		std::string name;
		name = m_universe->m_componentManager->GetComponent<EntityName>(id).name;
		PRINT("INFO", "entity name:", name);
		auto entityComponents = m_universe->m_entityManager->GetSignature(id);
		PRINT("INFO", "attached components lists");
		for (size_t i = 0; i < MAX_COMPONENTS; i++)
		{
			if (entityComponents.test(i))
			{
				PRINT("    ", m_universe->m_componentManager->GetComponentTypeName(i));
			}
		}
		PRINT("INFO", "attached systems list");
		for(size_t i = 0; i < MAX_COMPONENTS; i++ )
		{
			if (entityComponents.test(i))
			{
				//PRINT("    ", "for components:", m_universe->m_componentManager->GetComponentTypeName(i));
				ComponentSignature sig;
				sig.set(i);
				std::string systems = m_universe->m_systemManager->GetAttachedSystemNames(id, sig);
				PRINT("    ", systems);
			}
			
		}
	}

	~Entity()
	{
		//don't. Let the user manually destroy their entity.
		//m_universe->m_entityManager->DestroyEntity(id);
	}
private:
	Entity(Universe* universe, EntityId _id) : m_universe(universe), id(_id) {}
	EntityId id = INVALID_ENTITY;
	Universe* m_universe = nullptr;
};

