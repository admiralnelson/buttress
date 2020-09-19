#include "pch.h"
#include "ECS.h"
#include "components/EntityName.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "system/RenderSystem.h"
#include "system/EntityNameCheckSystem.h"

Entity Universe::CreateEntity(std::string name)
{
#pragma warning (push)
#pragma warning (disable : 26444) 
	auto entityChecker = m_systemManager->GetSystem<EntityNameCheckSystem>();
	if (entityChecker->CheckNow(name))
	{
		PRINT("ERROR", "duplicate entity name:", name);
		Entity ent(nullptr, INVALID_ENTITY);
		return ent;
	}
	Entity ent(this, m_entityManager->CreateEntity());
	EntityName entityName = { name };
	Transform t;
	ent.AddComponent<EntityName>(entityName);
	ent.AddComponent<Transform>(t);
	return ent;
#pragma warning (pop)
}

Entity Universe::QueryByEntityId(EntityId id)
{
	Entity ent(this, id);
	return ent;
}

void Universe::Render(float dt)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_lastDt = dt;
	m_systemManager->GetSystem<RenderSystem>()->Tick();
}

Universe::Universe()
{
	m_componentManager->RegisterComponent<EntityName>();
	m_componentManager->RegisterComponent<Transform>();
	m_componentManager->RegisterComponent<Mesh>();
	m_componentManager->RegisterComponent<Camera>();
	
	//for name check
	m_systemManager->RegisterSystem<EntityNameCheckSystem>(this);
	ComponentSignature nameSig;
	nameSig.set(m_componentManager->GetComponentType<EntityName>());
	m_systemManager->SetSignature<EntityNameCheckSystem>(nameSig);

	//for mesh render
	m_systemManager->RegisterSystem<RenderSystem>(this);
	ComponentSignature nameSig2;
	nameSig2.set(m_componentManager->GetComponentType<Transform>());
	nameSig2.set(m_componentManager->GetComponentType<Mesh>());
	m_systemManager->SetSignature<RenderSystem>(nameSig2);

	//for camera system
	m_systemManager->RegisterSystem<CameraSystem>(this);
	ComponentSignature nameSig3;
	nameSig3.set(m_componentManager->GetComponentType<Transform>());
	nameSig3.set(m_componentManager->GetComponentType<Camera>());
	m_systemManager->SetSignature<CameraSystem>(nameSig3);

}
