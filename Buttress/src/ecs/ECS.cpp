#include "pch.h"
#include "ECS.h"
#include "components/EntityName.h"


Entity Universe::CreateEntity(std::string name)
{
	if (m_nameCheckerSystem->CheckNow(name))
	{
		PRINT("ERROR", "duplicate entity name:", name);
		Entity ent(nullptr, INVALID_ENTITY);
		return ent;
	}
	Entity ent(this, m_entityManager->CreateEntity());
	EntityName entityName = { name };
	ent.AddComponent<EntityName>(entityName);
	return ent;
}

Entity Universe::QueryByEntityId(EntityId id)
{
	Entity ent(this, id);
	return ent;
}

Universe::Universe()
{
	m_componentManager->RegisterComponent<EntityName>();
	m_nameCheckerSystem = m_systemManager->RegisterSystem<EntityNameCheckSystem>(this);
	ComponentSignature nameSig;
	nameSig.set(m_componentManager->GetComponentType<EntityName>());
	m_systemManager->SetSignature<EntityNameCheckSystem>(nameSig);
}
