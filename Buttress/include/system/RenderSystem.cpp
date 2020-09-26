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
	std::lock_guard<std::mutex> mu(m_mutex);
	if (m_isFirstTick)
	{
		CameraSystem *cam = m_universe->GetSystem<CameraSystem>();
		m_camera = cam->FindPrimaryCamera();
		m_isFirstTick = false;

		glEnable(GL_DEPTH_TEST);
		///glEnable(GL_CULL_FACE);
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
	auto t2 = GetCurrentTime();
	PRINT("traverse time (ms)", t2-t1);

	auto trender1 = GetCurrentTime();
	RenderTheQueue();
	auto trender2 = GetCurrentTime();
	
	PRINT("render time (ms)", trender2 - trender1);
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
		if (ent.IsComponentExist<Animation>())
		{
			Animation& anim = ent.GetComponent<Animation>();
			if (anim.calculatedBonesMatrix.size() == 0)
			{
				return false;
			}
			queue.bonesTransformations = std::vector<Matrix4>(anim.calculatedBonesMatrix);
		}
		queue.meshId = meshId;
		queue.model = model;
		queue.projection = projection;
		queue.view = view;
		m_renderqueues[mat.shader][mesh.GetMaterialId()].push_back(queue);
	}

	return false;
}

static Shader* currentShader = nullptr;
static MaterialData* currentMaterial = nullptr;

void RenderSystem::RenderTheQueue()
{
	glClearColor(0.3, 0.4, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto &shader : m_renderqueues)
	{
		if (currentShader != shader.first.get())
		{
			shader.first->Use();
			currentShader = shader.first.get();
		}
		
		for (auto &materialData : shader.second)
		{
			//auto t1 = GetCurrentTime();
			MaterialData* mat = &MaterialLoader::GetMaterialById(materialData.first);
			if (currentMaterial != mat)
			{
				mat->Use();
				currentMaterial = mat;
			}
			for (auto& singleMesh : materialData.second)
			{
				//sets the animation matrix if shader has animation uniforms and queue contains animation transform
				if (shader.first->IsUniformArrayDefined(UNIFORM_ARRAY_MATRIX4_BONES))
				{
					size_t len = singleMesh.bonesTransformations.size();
					shader.first->SetUniformMat4x4Array(UNIFORM_ARRAY_MATRIX4_BONE"[0]", len, singleMesh.bonesTransformations[0]);
					//for (size_t  i = 0; i < len; i++)
					//{
					//	//std::string uniform = UNIFORM_ARRAY_MATRIX4_BONE"[" + std::to_string(i) + "]";
					//	//shader.first->SetUniformMat4x4(uniform, singleMesh.bonesTransformations[i]);
					//}
				}
				MeshLoader::GetMesh(singleMesh.meshId).Draw(singleMesh.projection, singleMesh.view, singleMesh.model);
				materialData.second.pop_front();
			}
			//instanced rendering?

			//PRINT("render objects dt(ms)", GetCurrentTime() - t1);
		}
		
	}
}
