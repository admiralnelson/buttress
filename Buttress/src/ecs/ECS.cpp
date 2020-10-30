#include "pch.h"
#include "ECS.h"
#include "components/EntityName.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "components/Node.h"
#include "components/Terrain.h"
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

static bool multiThread = true;

void Universe::Render(float dt)
{

	auto renderer = m_systemManager->GetSystem<RenderSystem>();
	auto animator = m_systemManager->GetSystem<AnimationSystem>();
	
	
	size_t entitiesInRenderer = renderer->GetTotalEntity();
	size_t entitiesInAnimator = animator->GetTotalEntity();


	if (multiThread)
	{
		tbb::parallel_for(tbb::blocked_range<int>(0, entitiesInRenderer),
			[&](tbb::blocked_range<int> r)
		{
			for (int i = r.begin(); i < r.end(); ++i)
			{
				renderer->Process(i);
			}
		});

		tbb::parallel_for(tbb::blocked_range<int>(0, entitiesInAnimator),
			[&](tbb::blocked_range<int> r)
		{
			for (int i = r.begin(); i < r.end(); ++i)
			{
				animator->Process(i);
			}
		});
	}
	else
	{
		renderer->TraverseTheGraph();
		animator->Tick(dt);
	}

	renderer->Tick();
	//m_workers.Reset();
	m_lastDt = dt;	
}

Universe::Universe()
{
	m_componentManager->RegisterComponent<EntityName>();
	m_componentManager->RegisterComponent<Transform>();
	m_componentManager->RegisterComponent<Model>();
	m_componentManager->RegisterComponent<Camera>();
	m_componentManager->RegisterComponent<Node>();
	m_componentManager->RegisterComponent<Animation>();
	m_componentManager->RegisterComponent<Terrain>();
	m_componentManager->RegisterComponent<Terrain::TerrainNode>();
	//for name check
	m_systemManager->RegisterSystem<EntityNameCheckSystem>(this);
	ComponentSignature entitySignature;
	entitySignature.set(m_componentManager->GetComponentType<EntityName>());
	m_systemManager->SetSignature<EntityNameCheckSystem>(entitySignature);


	//for animation
	m_systemManager->RegisterSystem<AnimationSystem>(this);
	ComponentSignature animationSig;
	animationSig.set(m_componentManager->GetComponentType<Transform>());
	animationSig.set(m_componentManager->GetComponentType<Model>());
	animationSig.set(m_componentManager->GetComponentType<Node>());
	animationSig.set(m_componentManager->GetComponentType<Animation>());
	m_systemManager->SetSignature<AnimationSystem>(animationSig);


	//for mesh render
	m_systemManager->RegisterSystem<RenderSystem>(this);
	ComponentSignature renderSig;
	renderSig.set(m_componentManager->GetComponentType<Transform>());
	renderSig.set(m_componentManager->GetComponentType<Model>());
	renderSig.set(m_componentManager->GetComponentType<Node>());
	m_systemManager->SetSignature<RenderSystem>(renderSig);

	//for camera system
	m_systemManager->RegisterSystem<CameraSystem>(this);
	ComponentSignature cameraSig;
	cameraSig.set(m_componentManager->GetComponentType<Transform>());
	cameraSig.set(m_componentManager->GetComponentType<Camera>());
	m_systemManager->SetSignature<CameraSystem>(cameraSig);

	m_tschedSetup = new tbb::task_scheduler_init(16);

}
