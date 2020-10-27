#include "pch.h"
#include "system/RenderSystem.h"
#include "system/AnimationSystem.h"
#include "components/Node.h"
#include "components/Animation.h"
#include "system/TerrainRenderSystem.h"
#include "Util.h"

static int freeMem;

void RenderSystem::Init(Universe* universe)
{
	m_universe = universe;
	
}


size_t RenderSystem::GetTotalEntity()
{
	return m_entity.size();
}


void RenderSystem::Process(size_t index)
{
	EntityId id = *std::next(m_entity.begin(), index);
	if (id == 0)
	{
		PRINT("WARNING!");
	}
	Transform& transform = m_universe->QueryByEntityId(id).GetComponent<Transform>();
	Node& node = m_universe->QueryByEntityId(id).GetComponent<Node>();
	if (node.parent == INVALID_ENTITY)
	{
		TraverseGraphForRender(id, transform.GetTransform());
	}
}

void RenderSystem::Tick()
{
	if (m_isFirstTick)
	{
		CameraSystem* cam = m_universe->GetSystem<CameraSystem>();
		m_camera = cam->FindPrimaryCamera();
		m_isFirstTick = false;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &freeMem);
		PRINT("INFO", "free mem is:", freeMem);

		std::array<int, 8> lodRange = { 1750, 874, 386, 192, 100, 50, 0, 0 };

		TerrainConfig config(0.0f, 6000, lodRange);

		TerrainRenderSystem::Instance().AddTerrain(config, m_camera);
	}

	auto sortInstance = [](const MeshQueue& a, const MeshQueue& b)
	{
		return  b.shader < a.shader && a.materialId < b.materialId || a.meshId < b.meshId;
	};

	while (!m_meshQueues.empty())
	{
		MeshQueue queue;
		if (!m_meshQueues.try_pop(queue))
		{
			continue;
		}
		m_sortedMeshQueues.emplace_back(queue);
	}
	//try deque the queue
	//insert to the real render queu!
	//then sort it
	std::sort(m_sortedMeshQueues.begin(), m_sortedMeshQueues.end(), sortInstance);

	RenderTheQueue();
	
}


bool RenderSystem::TraverseGraphForRender(EntityId e, const Matrix4& model)
{
	if (!m_camera.IsValid())
	{
		return false;
	}
	//travel recursively (DFS)
	Node& node = m_universe->QueryByEntityId(e).GetComponent<Node>();
	Entity ent = m_universe->QueryByEntityId(e);
	for (auto n : node.childs)
	{
		Matrix4 childModel = m_universe->QueryByEntityId(n.GetId()).GetComponent<Transform>().GetTransform();
		TraverseGraphForRender(n.GetId(), model * childModel);
	}

	
	if (m_universe->QueryByEntityId(e).IsComponentExist<Model>())
	{
		Model& modelComp = m_universe->QueryByEntityId(e).GetComponent<Model>();
		if (!ModelLoader::Instance().IsModelLoaded(modelComp.id))
		{
			m_busy = true;
			modelComp.id = ModelLoader::Instance().LoadModel(modelComp.objectPath, ent);
			m_busy = false;
		}

		//set the camera projection & view
		Matrix4 projection = m_universe->GetSystem<CameraSystem>()->Projection(m_camera);
		Matrix4 view = m_universe->GetSystem<CameraSystem>()->View(m_camera);
		ModelData& modelData = ModelLoader::Instance().GetModel(modelComp.id);
		bool sort = false;
		for (auto& meshId : modelData.m_meshes)
		{
			MeshData& mesh = MeshLoader::Instance().GetMesh(meshId);
			MaterialData& mat = MaterialLoader::Instance().GetMaterialById(mesh.GetMaterialId());
			MeshQueue queue;

			if (ent.IsComponentExist<Animation>())
			{
				Animation& anim = ent.GetComponent<Animation>();
				if (anim.calculatedBonesMatrix.size() == 0)
				{
					return false;
				}
				queue.bonesTransformations = std::vector<Matrix4>(anim.calculatedBonesMatrix);
			}
			queue.shader = mat.shader;
			queue.materialId = mesh.GetMaterialId();
			queue.meshId = meshId;
			queue.view = view;
			queue.model = model;
			queue.projection = projection;
			queue.entity = e;
			m_meshQueues.emplace(queue);
		}
	}
	return false;
}

static Shader* currentShader = nullptr;
static MaterialData* currentMaterial = nullptr;
static MeshData* currentMeshData = nullptr;
void RenderSystem::RenderTheQueue()
{
	glClearColor(0.3, 0.4, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	while(!m_sortedMeshQueues.empty())
	{
		MeshQueue &queue = m_sortedMeshQueues.front();

		if (queue.meshId == INVALID_MESH)
		{
			continue;
		}

		if (queue.materialId == INVALID_MATERIAL)
		{
			continue;
		}

		if (queue.shader == nullptr)
		{
			continue;
		}
		if (queue.shader.get() != currentShader)
		{
			currentShader = queue.shader.get();
			currentShader->Use();
		}

		if (!MaterialLoader::Instance().IsMaterialExist(queue.materialId))
		{
			continue;
		}
		
		Entity entity = m_universe->QueryByEntityId(queue.entity);
		bool isPlainModel = entity.IsComponentExist<Model>();

		MaterialData& material = MaterialLoader::Instance().GetMaterialById(queue.materialId);
		if (&material != currentMaterial && isPlainModel)
		{
			currentMaterial = &material;
			material.Use();
		}

		MeshData& mesh = MeshLoader::Instance().GetMesh(queue.meshId);
		if (&mesh != currentMeshData && isPlainModel)
		{
			currentMeshData = &mesh;
			mesh.Use();
		}
		
		std::vector<Matrix4>& bones = queue.bonesTransformations;
		if (bones.size() > 0)
		{
			queue.shader->SetUniformMat4x4Array(UNIFORM_ARRAY_MATRIX4_BONE"[0]", bones.size(), bones[0]);
		}

		if (isPlainModel)
		{
			mesh.Draw(queue.projection, queue.view, queue.model);
		}
		
		m_sortedMeshQueues.pop_front();
	}
	
}

void RenderSystem::TraverseTheGraph()
{
	for (auto& e : m_entity)
	{
		Transform& transform = m_universe->QueryByEntityId(e).GetComponent<Transform>();
		Node& node = m_universe->QueryByEntityId(e).GetComponent<Node>();
		if (node.parent == INVALID_ENTITY)
		{
			TraverseGraphForRender(e, transform.GetTransform());
		}
	}

}
