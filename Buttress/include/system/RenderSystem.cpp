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
			TraverseGraphForRender(e, transform.GetTransform());
		}
	}
}

void RenderSystem::Enqueue(std::shared_ptr<Shader> shader, MaterialData material, MeshData& mesh)
{
	
}

void RenderSystem::AddMaterial(MaterialData materialData)
{

}

MaterialId RenderSystem::GetMaterialId(MaterialData materialData)
{
	for (MaterialId i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i] == materialData)
		{
			return i;
		}
	}
	PRINT("WARN", "unable to find material ", materialData.name);
	return INVALID_MATERIAL;
}


bool RenderSystem::TraverseGraphForRender(EntityId e, Matrix4 model)
{
	//travel recursively (DFS)
	Node &node = m_universe->QueryByEntityId(e).GetComponent<Node>();
	for (auto n : node.childs)
	{
		Matrix4 childModel = m_universe->QueryByEntityId(n.GetId()).GetComponent<Transform>().GetTransform();
		TraverseGraphForRender(n.GetId(), model * childModel);
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

void RenderSystem::RenderTheQueue()
{
	std::unordered_map<int, MaterialData> x;
	

	for (auto &i : m_renderqueues)
	{
		i.first->Use();
		for (auto &j : i.second)
		{
			
		}
	}
}
