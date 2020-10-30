#pragma once
#include "pch.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "EventManager.h"
#include "components/EntityName.h"
#include "components/Node.h"

#ifdef __INTELLISENSE__
#pragma diag_suppress 26444
#endif

const unsigned MAX_JOB = 4;

class Entity;
class EntityNameCheckSystem;
struct EntityName;
class Universe
{
	friend class Entity;
	friend class Buttress;
public:
	Entity CreateEntity(std::string name);
	Entity QueryByEntityId(EntityId id);
	Entity QueryByName(std::string name);
	
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
	SYSTEM_TYPE* GetSystem() const
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
		m_running = false;
		PRINT("INFO", "the end times has come... (universe destroyed)", this);
	}
	void MemoryDebug()
	{
		PRINT("MEMORY DIAGNOSTIC");
		m_entityManager->MemoryDebug();
		m_componentManager->MemoryDebug();
		m_systemManager->MemoryDebug();
	}

	float GetLastDeltaTime() const { return m_lastDt; }

	unsigned int GetTotalEntities() 
	{ 
		return m_entityManager->GetTotalEntities(); 
	}

	
private:
	void Render(float dt);
	std::unique_ptr<ComponentManager> m_componentManager = std::make_unique<ComponentManager>();
	std::unique_ptr<EntityManager> m_entityManager       = std::make_unique<EntityManager>();
	std::unique_ptr<SystemManager> m_systemManager       = std::make_unique<SystemManager>();
	std::unique_ptr<EventManager> m_eventManager = std::make_unique<EventManager>();
	float m_lastDt = 0;
	bool m_running = true;
	tbb::task_scheduler_init* m_tschedSetup;
	std::recursive_mutex m_mutex;
};
class Entity
{
	friend class Universe;
public:
	Entity() : id(INVALID_ENTITY), m_universe(nullptr)
	{

	}

	Entity(const Entity& ent): id(ent.id), m_universe(ent.m_universe)
	{
		
	}

	friend bool operator==(const Entity& a, const Entity& b)
	{
		return a.id == b.id && a.m_universe == b.m_universe;
	}

	//Entity operator=(const Entity& otherEntity)
	//{
	//	Entity ent;
	//	ent.id = otherEntity.id;
	//	ent.m_universe = otherEntity.m_universe;
	//	return ent;
	//}

	// id could be stale!

	Entity CreateEntity(std::string name)
	{
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		Entity e;
		{
			e = m_universe->CreateEntity(name);
		}
		if (e.id == INVALID_ENTITY)
		{
			PRINT("ERROR", "ERROR SPAWNING ENTITY");
			throw std::runtime_error("error spawning entitiy");
		}
		return e;
	}

	bool IsValid()
	{
		return id != INVALID_ENTITY && m_universe != nullptr;
	}
	template<typename COMPONENT_TYPE>
	void AddComponent(COMPONENT_TYPE component)
	{
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::runtime_error("invalid entity");
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
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::runtime_error("invalid entity");
		}
		if (strcmp(typeid(COMPONENT_TYPE).name(), typeid(EntityName).name()) == 0)
		{
			PRINT("ERROR", "attempt to remove core component (EntityName) from entity! entity id:", id);
			throw std::runtime_error("attempt to remove core system from entity");
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
			throw std::runtime_error("invalid entity");
		}
		return m_universe->m_componentManager->GetComponent<COMPONENT_TYPE>(id);
	}

	template<typename COMPONENT_TYPE>
	bool IsComponentExist()
	{
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::runtime_error("invalid entity");
		}
		return m_universe->m_componentManager->IsComponentExistForEntity<COMPONENT_TYPE>(id);
	}

	template<typename SYSTEM_TYPE>
	void AttachToSystem()
	{
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::runtime_error("invalid entity");
		}
		ComponentSignature objectCompSig = m_universe->m_entityManager->GetSignature(id);
		objectCompSig = objectCompSig | m_universe->m_systemManager->GetSignature<SYSTEM_TYPE>();
		m_universe->m_entityManager->SetSignature(id, objectCompSig);
	}
	

	void AttachChild(Entity entity)
	{
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		if (entity.id == INVALID_ENTITY)
		{
			PRINT("ERROR", "invalid entity is attached!");
			throw std::runtime_error("invalid entity");
		}
		Node& parentNode = GetComponent<Node>();
		Node& childNode = entity.GetComponent<Node>();
		if (ContainChild(entity))
		{
			return;
		}
		if (parentNode.mainParent == INVALID_ENTITY)
		{
			childNode.mainParent = id;
		}
		else
		{
			childNode.mainParent = parentNode.mainParent;
		}
		childNode.parent = id; //set parent in the child
		parentNode.childs.push_back(entity); //then push the child to parent child list
	}

	void RemoveChild(Entity entity)
	{
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		if (entity.id == INVALID_ENTITY)
		{
			PRINT("ERROR", "invalid entity is removed!");
			throw std::runtime_error("invalid entity");
		}
		Node& parentNode = GetComponent<Node>();
		Node& childNode = entity.GetComponent<Node>();
		if (!ContainChild(entity))
		{
			return;
		}
		childNode.parent = INVALID_ENTITY; //set parent in the none
		parentNode.childs.erase(std::remove(parentNode.childs.begin(), parentNode.childs.end(), entity)); //then remove the child from parent child list
	}

	template<typename SYSTEM_TYPE>
	void RemoveFromSystem()
	{
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		if (id == INVALID_ENTITY)
		{
			PRINT("ERROR", "INVALID ENTITY!");
			throw std::runtime_error("invalid entity");
		}
		if (strcmp(typeid(SYSTEM_TYPE).name(), typeid(EntityNameCheckSystem).name()) == 0)
		{
			PRINT("ERROR", "attempt to remove core system (EntityNameCheckSystem) from entity! entity id:", id);
			throw std::runtime_error("attempt to remove core system from entity");
		}
		ComponentSignature objectCompSig = m_universe->m_entityManager->GetSignature(id);
		objectCompSig = objectCompSig & m_universe->m_systemManager->GetSignature<SYSTEM_TYPE>();
		m_universe->m_entityManager->SetSignature(id, objectCompSig);
	}

	EntityId const GetId() const { return id; }
	
	bool ContainChild(Entity child)
	{
		//search recursively (DFS)
		Node& parentNode = GetComponent<Node>();
		bool doesntExistInParent = std::find(parentNode.childs.begin(), parentNode.childs.end(), child) == parentNode.childs.end();
		for (auto c : parentNode.childs)
		{
			bool doesntExistInChildren = c.ContainChild(child);
			doesntExistInParent |= doesntExistInChildren;
		}
		return !doesntExistInParent;
	}

	Entity GetChild(unsigned int idx)
	{
		Node& node = GetComponent<Node>();
		return node.GetChild(idx);
	}

	void Destroy()
	{
		std::lock_guard<std::recursive_mutex> lock(m_universe->m_mutex);
		Node &parentNode = GetComponent<Node>();
		for (auto child : parentNode.childs)
		{
			child.Destroy();
		}

		m_universe->m_entityManager->DestroyEntity(id);
		id = INVALID_ENTITY;
	}

	/// O(N*2) operation!
	void Debug()
	{
		std::string name;
		name = m_universe->m_componentManager->GetComponent<EntityName>(id).name;
		PRINT("INFO", "entity name:", name);
		auto entityComponents = m_universe->m_entityManager->GetSignature(id);
		PRINT("INFO", "attached components lists");
		for (ComponentTypeId i = 0; i < MAX_COMPONENTS; i++)
		{
			if (entityComponents.test(i))
			{
				PRINT("    ", m_universe->m_componentManager->GetComponentTypeName(i));
			}
		}
		PRINT("INFO", "trees");
		PrintNodeRecursively(*this, 0);
	}

private:
	void PrintNodeRecursively(Entity& e, int indent)
	{
		Node& node = e.GetComponent<Node>();
		for (size_t i = 0; i < node.childs.size(); i++)
		{
			e.PrintNodeRecursively(node.childs[i], indent + 1);
		}
		std::string name = e.GetComponent<EntityName>().name;
		std::string spacer;
		for (size_t i = 0; i < indent; i++)
		{
			spacer += "-";
		}
		PRINT("    ", spacer, name);
	}
	Entity(Universe* universe, EntityId _id) : m_universe(universe), id(_id) {}
	EntityId id = INVALID_ENTITY;
	Universe* m_universe = nullptr;
};

