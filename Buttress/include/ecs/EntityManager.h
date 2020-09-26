#pragma once
#include "pch.h"

class EntityManager
{
public:
	EntityManager();
	EntityId CreateEntity();
	void DestroyEntity(EntityId entity);
	void SetSignature(EntityId entity, ComponentSignature signature);
	ComponentSignature GetSignature(EntityId entity);
	~EntityManager()
	{
		PRINT("INFO","entity manager destroyed", this);
	}
	void MemoryDebug()
	{
		PRINT("MEMORY: ECS", this, "object. m_entities allocates", sizeof(std::queue<EntityId>) + (sizeof(EntityId) * m_entities.size()) );
		PRINT("MEMORY: ECS", this, "object. m_signatures allocates", sizeof(std::vector<ComponentSignature>) + ((sizeof(ComponentSignature) * m_signatures.size())));
	}
private:
	//available entity slot
	std::queue<EntityId> m_entities{};
	//component signature for every entities available
	std::vector<ComponentSignature> m_signatures{};
	int m_entitiesCount = 0;
	std::mutex m_mutex;
};