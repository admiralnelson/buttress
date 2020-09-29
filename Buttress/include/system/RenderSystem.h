#pragma once
#include "ecs/ECS.h"
#include "components/Transform.h"
#include "components/Model.h"
#include "system/CameraSystem.h"
#include "core/Material.h"
#include "core/ModelData.h"
#include "core/MeshData.h"
#include "glad/glad.h"
#include "components/Transform.h"
class Universe;

struct MeshQueue
{
	std::shared_ptr<Shader> shader;
	MaterialId materialId = INVALID_MATERIAL;
	MeshId meshId = INVALID_MESH;
	Matrix4 view;
	Matrix4 model;
	Matrix4 projection;
	std::vector<Matrix4> bonesTransformations;
};



class ModelData;
//class MeshLoader;
//class MaterialLoader;
class MaterialData;
class RenderSystem : public System
{
	friend class Universe;
	friend class AnimationSystem;
public:
	void Init(Universe* universe) override;
	void Tick();
private:
	bool TraverseGraphForRender(EntityId e, Matrix4 model);
	//main thread
	void RenderTheQueue();
	//secondary thread
	void TraverseTheGraph();
	std::deque<MeshQueue> m_meshQueues;
	std::mutex m_mutex;
	std::thread m_traversalThread;
	std::thread m_animationSystem;
	Entity m_camera;
	bool m_isFirstTick = true;
	bool m_busy = false;
	bool m_render = false;
	long long m_sceneGraphSleepForMs = 0;
	//std::vector<unsigned int> m_bonesTransforms;
};