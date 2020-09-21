#include "pch.h"
#include "RenderSystem.h"
#include "components/Node.h"

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
		Transform& transform = m_universe->QueryByEntityId(e).GetComponent<Transform>();
		Node& node = m_universe->QueryByEntityId(e).GetComponent<Node>();
		//draw the parent
		if (node.parent == INVALID_ENTITY)
		{
			Render(e, transform.GetTransform());
		}
	}
}

bool RenderSystem::RenderModel(std::string objPath, Transform entityTransform)
{
	if (m_models.find(objPath) == m_models.end())
	{
		m_models[objPath] = ModelData(objPath);
	}
	//set the camera projection & view
	Matrix4 projection;
	projection = m_universe->GetSystem<CameraSystem>()->Projection(m_camera);
	Matrix4 view;
	view = m_universe->GetSystem<CameraSystem>()->View(m_camera);	
	m_models[objPath].Draw(projection, view, entityTransform.GetTransform());
	return false;
}

bool RenderSystem::Render(EntityId e, Matrix4 model)
{
	//travel recursively (DFS)
	Node &node = m_universe->QueryByEntityId(e).GetComponent<Node>();
	for (auto n : node.childs)
	{
		Matrix4 childModel = m_universe->QueryByEntityId(n.GetId()).GetComponent<Transform>().GetTransform();
		Render(n.GetId(), model * childModel);
	}

	Mesh mesh = m_universe->QueryByEntityId(e).GetComponent<Mesh>();
	if (m_models.find(mesh.objectPath) == m_models.end())
	{
		m_models[mesh.objectPath] = ModelData(mesh.objectPath);
	}
	//set the camera projection & view
	Matrix4 projection;
	projection = m_universe->GetSystem<CameraSystem>()->Projection(m_camera);
	Matrix4 view;
	view = m_universe->GetSystem<CameraSystem>()->View(m_camera);
	m_models[mesh.objectPath].Draw(projection, view, model);

	return false;
}
