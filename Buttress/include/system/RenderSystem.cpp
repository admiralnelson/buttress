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
	auto t1 = std::chrono::high_resolution_clock::now();
	RenderTheQueue();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	//PRINT("render time (ms)", duration);
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

	Mesh &mesh = m_universe->QueryByEntityId(e).GetComponent<Mesh>();
	if (mesh.id < 0 || mesh.id > m_modelsPaths.size())
	{
		m_modelsPaths.push_back(mesh.objectPath);
		mesh.id = (ModelId) m_modelsPaths.size() - 1;
		m_models.push_back(ModelData(mesh.objectPath));
	}
	//set the camera projection & view
	Matrix4 projection = m_universe->GetSystem<CameraSystem>()->Projection(m_camera);
	Matrix4 view = m_universe->GetSystem<CameraSystem>()->View(m_camera);
	for (auto& meshId : m_models[mesh.id].m_meshes)
	{
		MeshData& mesh = MeshLoader::GetMesh(meshId);
		MaterialData& mat = MaterialLoader::GetMaterialById(mesh.GetMaterialId());
		MeshQueue queue;
		queue.meshId = meshId;
		queue.model = model;
		queue.projection = projection;
		queue.view = view;
		m_renderqueues[mat.shader][mesh.GetMaterialId()].push_back(queue);
	}

	return false;
}

void RenderSystem::RenderTheQueue()
{
	for (auto &i : m_renderqueues)
	{
		i.first->Use();
		for (auto &j : i.second)
		{
			MaterialLoader::GetMaterialById(j.first).Use();
			for (auto& k : j.second)
			{

				{
					MeshLoader::GetMesh(k.meshId).Draw(k.projection, k.view, k.model);
				}
				j.second.pop_front();
			}
		}
	}
}