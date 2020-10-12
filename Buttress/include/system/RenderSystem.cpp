#include "pch.h"
#include "RenderSystem.h"
#include "system/AnimationSystem.h"
#include "components/Node.h"
#include "components/Animation.h"
#include "Util.h"

static int freeMem;

void RenderSystem::Init(Universe* universe)
{
	m_universe = universe;
	
}

void RenderSystem::ProcessJob(jobsystem::ThreadNr jobIndex, jobsystem::NrOfThreads totalThreads)
{
	const int nrOfitems = m_entity.size();
	const int start = jobIndex * nrOfitems / totalThreads;
	const int finish = std::min((jobIndex + 1) * nrOfitems / totalThreads, (unsigned int)m_entity.size());
	for (unsigned int i = start; i < finish; i++)
	{
		EntityId id = *std::next(m_entity.begin(), i);
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
	}

	auto sortInstance = [](const MeshQueue& a, const MeshQueue& b)
	{
		return  b.shader < a.shader && a.materialId < b.materialId || a.meshId < b.meshId;
	};

	//try deque the queue
	//insert to the real render queu!
	//then sort it
	//std::sort(m_meshQueues.begin(), m_meshQueues.end(), sortInstance);

	RenderTheQueue();
}


bool RenderSystem::TraverseGraphForRender(EntityId e, Matrix4 model)
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

	
	if (!m_universe->QueryByEntityId(e).IsComponentExist<Model>())
	{
		return false;
	}
	Model &modelComp = m_universe->QueryByEntityId(e).GetComponent<Model>();
	if (!ModelLoader::Instance().IsModelLoaded(modelComp.id))
	{
		m_busy = true;
		modelComp.id = ModelLoader::Instance().LoadModel(modelComp.objectPath, ent);
		m_busy = false;
	}

	//if (modelComp.id < 0 || modelComp.id > m_modelsPaths.size())
	//{
	//	m_modelsPaths.push_back(modelComp.objectPath);
	//	modelComp.id = (ModelId) m_modelsPaths.size() - 1;
	//	m_models.push_back(ModelData(modelComp.objectPath, ent));
	//}
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
		m_meshQueues.emplace(queue);
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
	while(!m_meshQueues.empty())
	{
		MeshQueue queue;
		if (!m_meshQueues.try_pop(queue))
		{
			continue;
		}

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

		MaterialData& material = MaterialLoader::Instance().GetMaterialById(queue.materialId);
		if (&material != currentMaterial)
		{
			currentMaterial = &material;
			material.Use();
		}

		MeshData& mesh = MeshLoader::Instance().GetMesh(queue.meshId);
		if (&mesh != currentMeshData)
		{
			currentMeshData = &mesh;
			mesh.Use();
		}

		std::vector<Matrix4>& bones = queue.bonesTransformations;
		if (bones.size() > 0)
		{
			queue.shader->SetUniformMat4x4Array(UNIFORM_ARRAY_MATRIX4_BONE"[0]", bones.size(), bones[0]);
		}

		mesh.Draw(queue.projection, queue.view, queue.model);
		
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
