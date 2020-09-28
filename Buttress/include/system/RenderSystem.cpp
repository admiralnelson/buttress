#include "pch.h"
#include "RenderSystem.h"
#include "components/Node.h"
#include "components/Animation.h"
#include "Util.h"
static int freeMem;

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

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &freeMem);
		PRINT("INFO", "free mem is:", freeMem);
	}
	auto t1 = GetCurrentTime();
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

	auto sortInstance = [](const MeshQueue& a, const MeshQueue& b)
	{
		return  b.shader < a.shader && a.materialId < b.materialId || a.meshId < b.meshId;
	};

	std::sort(m_meshQueues.begin(), m_meshQueues.end(), sortInstance);

	auto t2 = GetCurrentTime();
	//PRINT("traverse time (ms)", t2-t1);

	auto trender1 = GetCurrentTime();
	RenderTheQueue();
	auto trender2 = GetCurrentTime();
	//PRINT("render time (ms)", trender2 - trender1);
}


bool RenderSystem::TraverseGraphForRender(EntityId e, Matrix4 model)
{
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
	if (modelComp.id < 0 || modelComp.id > m_modelsPaths.size())
	{
		m_modelsPaths.push_back(modelComp.objectPath);
		modelComp.id = (ModelId) m_modelsPaths.size() - 1;
		m_models.push_back(ModelData(modelComp.objectPath, ent));
	}
	//set the camera projection & view
	Matrix4 projection = m_universe->GetSystem<CameraSystem>()->Projection(m_camera);
	Matrix4 view = m_universe->GetSystem<CameraSystem>()->View(m_camera);
	for (auto& meshId : m_models[modelComp.id].m_meshes)
	{
		MeshData& mesh = MeshLoader::GetMesh(meshId);
		MaterialData& mat = MaterialLoader::GetMaterialById(mesh.GetMaterialId());
		MeshQueue queue;
		
		queue.shader = mat.shader;
		queue.materialId = mesh.GetMaterialId();
		queue.meshId = meshId;
		queue.view = view;
		queue.model = model;
		queue.projection = projection;
		if (ent.IsComponentExist<Animation>())
		{
			Animation& anim = ent.GetComponent<Animation>();
			if (anim.calculatedBonesMatrix.size() == 0)
			{
				return false;
			}
			queue.bonesTransformations = std::vector<Matrix4>(anim.calculatedBonesMatrix);
		}
		m_meshQueues.push_back(queue); 
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
		MeshQueue& queue = m_meshQueues.front();
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

		MaterialData& material = MaterialLoader::GetMaterialById(queue.materialId);
		if (&material != currentMaterial)
		{
			currentMaterial = &material;
			material.Use();
		}

		MeshData& mesh = MeshLoader::GetMesh(queue.meshId);
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
		m_meshQueues.pop_front();
	}
}

void RenderSystem::LoadTheModelQueue()
{

}
