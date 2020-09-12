#pragma once
#include "pch.h"
#include "EntityManager.h"
#include "ComponentManager.h"

class Universe;

class System
{
	friend class SystemManager;
	virtual void Init(Universe* universe) = 0;
protected:
	std::set<EntityId> m_entity;
	Universe* m_universe = nullptr;
};

class SystemManager
{
public:
	template<typename SYSTEM_TYPE>
	std::shared_ptr<SYSTEM_TYPE> RegisterSystem(Universe *universe)
	{
		std::string name = typeid(SYSTEM_TYPE).name();

		if (m_systems.find(name) != m_systems.end())
		{
			PRINT("ERROR", "system", name, "has been already registered");
			throw std::exception("double register system ");
		}
		auto system = std::make_shared<SYSTEM_TYPE>();
		system->Init(universe);
		m_systems.insert({ name, system });
		return system;
	}

	template<typename SYSTEM_TYPE>
	void SetSignature(ComponentSignature signature)
	{
		std::string name = typeid(SYSTEM_TYPE).name();

		if (m_systems.find(name) == m_systems.end())
		{
			PRINT("ERROR", "cannot find system", name, "it is not registered");
			throw std::exception("attempt to use non exsiting system ");
		}
		m_signatures.insert({ name, signature });
	}

	template<typename SYSTEM_TYPE>
	ComponentSignature GetSignature()
	{
		std::string name = typeid(SYSTEM_TYPE).name();

		if (m_systems.find(name) == m_systems.end())
		{
			PRINT("ERROR", "cannot find system", name, "it is not registered");
			throw std::exception("attempt to use non exsiting system ");
		}

		return m_signatures[name];
	}

	void EntityDestroyed(EntityId entity);
	void EntitySignatureChanged(EntityId entity, ComponentSignature signature);
	~SystemManager()
	{
		PRINT("INFO", "system manager destroyed", this);
	}

	void MemoryDebug()
	{
		PRINT("TO BE IMPLEMENTED");
	}
private:
	//map system name to associated components using a bitset signature
	std::unordered_map<std::string, ComponentSignature> m_signatures;
	//map system name to the system itself
	std::unordered_map<std::string, std::shared_ptr<System>> m_systems;
};