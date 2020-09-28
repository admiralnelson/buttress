#include "pch.h"
#include "ECS.h"
#include "components/EntityName.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "components/Node.h"
#include "components/Animation.h"
#include "system/RenderSystem.h"
#include "system/AnimationSystem.h"
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
	ent.AddComponent<Node>(Node());
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
	auto t1 = std::chrono::high_resolution_clock::now();
	m_systemManager->GetSystem<RenderSystem>()->Tick();
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	
	m_lastDt = duration.count();
}

Universe::Universe()
{
	m_componentManager->RegisterComponent<EntityName>();
	m_componentManager->RegisterComponent<Transform>();
	m_componentManager->RegisterComponent<Model>();
	m_componentManager->RegisterComponent<Camera>();
	m_componentManager->RegisterComponent<Node>();
	m_componentManager->RegisterComponent<Animation>();
	//for name check
	m_systemManager->RegisterSystem<EntityNameCheckSystem>(this);
	ComponentSignature nameSig;
	nameSig.set(m_componentManager->GetComponentType<EntityName>());
	m_systemManager->SetSignature<EntityNameCheckSystem>(nameSig);


	//for animation
	m_systemManager->RegisterSystem<AnimationSystem>(this);
	ComponentSignature nameSig3;
	nameSig3.set(m_componentManager->GetComponentType<Transform>());
	nameSig3.set(m_componentManager->GetComponentType<Model>());
	nameSig3.set(m_componentManager->GetComponentType<Node>());
	nameSig3.set(m_componentManager->GetComponentType<Animation>());
	m_systemManager->SetSignature<AnimationSystem>(nameSig3);


	//for mesh render
	m_systemManager->RegisterSystem<RenderSystem>(this);
	ComponentSignature nameSig2;
	nameSig2.set(m_componentManager->GetComponentType<Transform>());
	nameSig2.set(m_componentManager->GetComponentType<Model>());
	nameSig2.set(m_componentManager->GetComponentType<Node>());
	m_systemManager->SetSignature<RenderSystem>(nameSig2);


	//for camera system
	m_systemManager->RegisterSystem<CameraSystem>(this);
	ComponentSignature nameSig4;
	nameSig4.set(m_componentManager->GetComponentType<Transform>());
	nameSig4.set(m_componentManager->GetComponentType<Camera>());
	m_systemManager->SetSignature<CameraSystem>(nameSig4);

	m_animationThread = std::thread([&]()
	{
		while (m_running)
		{
			//m_systemManager->GetSystem<AnimationSystem>()->Tick(0);
		}
	});
}
