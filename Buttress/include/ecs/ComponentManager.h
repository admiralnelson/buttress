#pragma once
#include "pch.h"
#include "EntityManager.h"
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(EntityId entity) = 0;
};

template<typename TYPE>
class ComponentArray : public IComponentArray
{
public:
	ComponentArray()
	{
		m_componentArray.resize(MAX_ENTITIES);
	}
	void InsertData(EntityId entity, TYPE component)
	{
		if (m_entityToIndexMap.find(entity) != m_entityToIndexMap.end())
		{
			PRINT("ERROR", "a component assigned to the same entity twice! component type:", typeid(TYPE).name());
			throw std::runtime_error("duplicate component assigment");
		}
		size_t newIndex = m_alive;
		m_entityToIndexMap[entity] = newIndex;
		m_indexToEntityMap[newIndex] = entity;
		m_componentArray[newIndex] = component;
		m_alive++;
	}
	void RemoveData(EntityId entity)
	{
		std::lock_guard<std::recursive_mutex> lock(m_eraseGuard);
		if (m_entityToIndexMap.find(entity) == m_entityToIndexMap.end())
		{
			PRINT("ERROR", "attempt to remove non existing entity");
			throw std::runtime_error("removing non existing enitty");
		}

		//shift the component array
		size_t indexOfRemovedEntity = m_entityToIndexMap[entity];
		size_t indexOfLastElement = m_alive - 1;
		m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];

		EntityId entityOfLastElement = m_indexToEntityMap[indexOfLastElement];
		m_entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		m_indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		m_entityToIndexMap.unsafe_erase(entity);
		m_indexToEntityMap.unsafe_erase(indexOfLastElement);

		m_alive--;
	}

	void Debug(EntityId entity)
	{

	}

	void EntityDestroyed(EntityId entity) override
	{
		if (m_entityToIndexMap.find(entity) != m_entityToIndexMap.end())
		{
			RemoveData(entity);
		}
	}

	TYPE& GetData(EntityId entity)
	{	
		if (m_entityToIndexMap.find(entity) == m_entityToIndexMap.end())
		{
			std::string typeName = typeid(TYPE).name();
			PRINT("ERROR", "attempt to retrieve non existing component:", typeName  ," from entity id:", entity);
			throw std::runtime_error("attempt to retrieve non existing component");
		}
		return m_componentArray[m_entityToIndexMap[entity]];
	}

	bool IsComponentExist(EntityId id)
	{
		if (m_entityToIndexMap.find(id) == m_entityToIndexMap.end())
		{
			return false;
		}
		return true;
	}

	void MemoryDebug()
	{
		//PRINT("MEMORY: ECS", this, "object. m_componentArray allocates", sizeof(std::vector<TYPE>) + (sizeof(TYPE) * m_componentArray.size()), "for component type of" + typeid(TYPE).name());
	}

private:
	std::recursive_mutex m_eraseGuard;
	//alocates component container with total element of MAX_ENTITIES, each entity can lookup a single component through here
	tbb::concurrent_vector<TYPE> m_componentArray;
	//maps entityId to its component index
	tbb::concurrent_unordered_map<EntityId, size_t, std::hash<EntityId>> m_entityToIndexMap;
	//maps component index to its corresponding entityId
	tbb::concurrent_unordered_map<size_t, EntityId, std::hash<size_t>> m_indexToEntityMap;
	int m_alive = 0;
};

class ComponentManager
{
public:
	template<typename COMPONENT_TYPE>
	void RegisterComponent()
	{
		
		std::string name = typeid(COMPONENT_TYPE).name();

		if (m_componentTypes.find(name) != m_componentTypes.end())
		{
			PRINT("ERROR", "registering the same component! component:", name);
			throw std::runtime_error("same component registered ");
		}
		m_componentTypes.insert({ name, m_nextComponentType });
		m_componentTypeNames.insert({ m_nextComponentType, name });
		m_componentArrays.insert({ name, std::make_shared<ComponentArray<COMPONENT_TYPE>>() });

		m_nextComponentType++;
	}

	template<typename COMPONENT_TYPE>
	ComponentTypeId GetComponentType()
	{
		
		std::string name = typeid(COMPONENT_TYPE).name();
		if (m_componentTypes.find(name) == m_componentTypes.end())
		{
			PRINT("ERROR", "component", name, "not registered!");
			throw std::runtime_error("component  was not registered");
		}

		return m_componentTypes[name];
	}

	std::string GetComponentTypeName(ComponentTypeId compId)
	{
		
		if (m_componentTypeNames.find(compId) == m_componentTypeNames.end())
		{
			PRINT("ERROR", "component with id:", compId, "not registered!");
			throw std::runtime_error("component  was not registered");
		}
		return m_componentTypeNames[compId];
	}


	template<typename COMPONENT_TYPE>
	void AddComponent(EntityId entity, COMPONENT_TYPE component)
	{
		GetComponentArray<COMPONENT_TYPE>()->InsertData(entity, component);
	}

	template<typename COMPONENT_TYPE>
	void RemoveComponent(EntityId entity)
	{
		GetComponentArray<COMPONENT_TYPE>()->RemoveData(entity);
	}

	template<typename COMPONENT_TYPE>
	COMPONENT_TYPE& GetComponent(EntityId entity)
	{
		return GetComponentArray<COMPONENT_TYPE>()->GetData(entity);
	}

	template<typename COMPONENT_TYPE>
	bool IsComponentExistForEntity(EntityId entity)
	{
		
		return GetComponentArray<COMPONENT_TYPE>()->IsComponentExist(entity);
	}

	void EntityDestroyed(EntityId entity) 
	{
		
		for (auto& i : m_componentArrays)
		{
			auto& comp = i.second;
			comp->EntityDestroyed(entity);
		}
	}
	void MemoryDebug()
	{
		PRINT("TO BE IMPLEMENTED")
	}

	~ComponentManager()
	{
		PRINT("INFO", "component manager destroyed", this);
	}
private:
	//maps component name to its typeId
	tbb::concurrent_unordered_map<std::string, ComponentTypeId, std::hash<std::string>> m_componentTypes;
	tbb::concurrent_unordered_map<ComponentTypeId, std::string, std::hash<ComponentTypeId>> m_componentTypeNames;
	//maps component name to its container
	tbb::concurrent_unordered_map<std::string, std::shared_ptr<IComponentArray>, std::hash<std::string>> m_componentArrays;
	tbb::concurrent_map<unsigned long long, std::string, std::hash<unsigned long long>> m_hashToComponentName;
	//next id of next registered component
	ComponentTypeId m_nextComponentType;
	//get component container based on its type
	template<typename COMPONENT_TYPE>
	std::shared_ptr<ComponentArray<COMPONENT_TYPE>> GetComponentArray() 
	{
		std::string name = typeid(COMPONENT_TYPE).name();
		auto tx = typeid(COMPONENT_TYPE).hash_code();
		if (m_componentTypes.find(name) == m_componentTypes.end())
		{
			PRINT("ERROR", "component", name, "is not registered");
			throw std::runtime_error("component  not registered");
		}

		return std::static_pointer_cast<ComponentArray<COMPONENT_TYPE>>(m_componentArrays[name]);
	}

};

