#include "pch.h"
#include "RenderSystem.h"

void RenderSystem::Init(Universe* universe)
{
	m_universe = universe;
}

void RenderSystem::Tick()
{
	if (m_isFirstTick)
	{
		CameraSystem *cam = m_universe->GetSystem<CameraSystem>();
		m_camera = cam->FindPrimaryCamera();
	}
	
	for (auto& e : m_entity)
	{
		Mesh& mesh = m_universe->QueryByEntityId(e).GetComponent<Mesh>();
		
	}
}

bool RenderSystem::LoadModel(std::string objPath)
{
	if (m_models.find(objPath) != m_models.end())
	{
		return false;
	}


	return true;
}
