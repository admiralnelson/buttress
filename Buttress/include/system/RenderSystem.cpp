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
		m_isFirstTick = false;
	}
	
	for (auto& e : m_entity)
	{
		Mesh mesh = m_universe->QueryByEntityId(e).GetComponent<Mesh>();
		Transform t = m_universe->QueryByEntityId(e).GetComponent<Transform>();
		RenderModel(mesh.objectPath, t);
	}
}

bool RenderSystem::RenderModel(std::string objPath, Transform entityTransform)
{
	if (m_models.find(objPath) == m_models.end())
	{
		m_models[objPath] = Model(objPath);
	}
	//set the camera projection & view
	Matrix4 projection;
	projection = m_universe->GetSystem<CameraSystem>()->Projection(m_camera);
	Matrix4 view;
	view = m_universe->GetSystem<CameraSystem>()->View(m_camera);	
	m_models[objPath].Draw(projection, view, entityTransform);
	return false;
}
