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
#include <Util.h>

Entity Universe::CreateEntity(std::string name)
{
#pragma warning (push)
#pragma warning (disable : 26444) 
	auto entityChecker = m_systemManager->GetSystem<EntityNameCheckSystem>();
	/*if (entityChecker->CheckNow(name))
	{
		PRINT("ERROR", "duplicate entity name:", name);
		Entity ent(nullptr, INVALID_ENTITY);
		return ent;
	}*/
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
	auto t1 = GetSystemTime();

	m_workers
		.Do(m_animationLoop, 'a')
		.Then()
		.Do(m_sceneGraphLoop, 'b')
		.Then()
		.Together();

	m_workers
		.Go()
		.AssistAndWaitForAll();
	
	//m_workers.BlockUntilFinished();
	auto t2 = GetSystemTime();
	//PRINT("delta ", t2 - t1);
	m_lastDt = dt;
	m_systemManager->GetSystem<RenderSystem>()->Tick();
	
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

	m_jobNames.m_workers.push_back("SceneGraph1");
	m_jobNames.m_workers.push_back("Animation1");

	m_jobNames.m_workers.push_back("SceneGraph2");
	m_jobNames.m_workers.push_back("Animation2");

	m_jobNames.m_workers.push_back("SceneGraph3");
	m_jobNames.m_workers.push_back("Animation3");

	m_jobNames.m_workers.push_back("SceneGraph4");
	m_jobNames.m_workers.push_back("Animation4");

	m_jobManager.Create(m_jobNames);

	m_animationLoop = ([&](jobsystem::ThreadNr threadNr, jobsystem::NrOfThreads numberOfThreads)
	{
		auto animation = m_systemManager->GetSystem<AnimationSystem>();
		animation->ProcessJob(threadNr, numberOfThreads);
	});
	m_sceneGraphLoop = ([&](jobsystem::ThreadNr threadNr, jobsystem::NrOfThreads numberOfThreads)
	{
		auto renderer = m_systemManager->GetSystem<RenderSystem>();
		renderer->ProcessJob(threadNr, numberOfThreads);
	});


}
