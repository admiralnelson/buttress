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
			throw std::runtime_error("double register system ");
		}
		auto system = std::make_shared<SYSTEM_TYPE>();
		system->Init(universe);
		m_systems.insert({ name, system });
		return system;
	}

	template<typename SYSTEM_TYPE>
	SYSTEM_TYPE* GetSystem()
	{
		
		std::string name = typeid(SYSTEM_TYPE).name();

		if (m_systems.find(name) == m_systems.end())
		{
			PRINT("ERROR", "system", name, "is not registered");
			throw std::runtime_error("system not registered");
		}
		//return m_systems[name];
		auto ptr = dynamic_cast<SYSTEM_TYPE*>(m_systems[name].get());
		return ptr;
	}

	template<typename SYSTEM_TYPE>
	void SetSignature(ComponentSignature signature)
	{
		
		std::string name = typeid(SYSTEM_TYPE).name();

		if (m_systems.find(name) == m_systems.end())
		{
			PRINT("ERROR", "cannot find system", name, "it is not registered");
			throw std::runtime_error("attempt to use non exsiting system ");
		}
		m_signatures.insert({ name, signature });
		//keep it 1-to-1
		if (m_signatures.find(name) != m_signatures.end())
		{
			ComponentSignature sig = m_signatures[name];
			m_signaturesToName.erase(sig);
		}
		m_signaturesToName.insert({ signature, name });

	}

	template<typename SYSTEM_TYPE>
	ComponentSignature GetSignature()
	{
		
		std::string name = typeid(SYSTEM_TYPE).name();

		if (m_systems.find(name) == m_systems.end())
		{
			PRINT("ERROR", "cannot find system", name, "it is not registered");
			throw std::runtime_error("attempt to use non exsiting system ");
		}

		return m_signatures[name];
	}

	std::string GetAttachedSystemNames(EntityId entity, ComponentSignature componentSignature);


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
	//map associated components to system name
	std::unordered_map<ComponentSignature, std::string> m_signaturesToName;
	//map system name to the system itself
	std::unordered_map<std::string, std::shared_ptr<System>> m_systems;
};