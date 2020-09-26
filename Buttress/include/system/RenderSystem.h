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
	MaterialId GetMaterialId(MaterialData materialData);

private:
	bool TraverseGraphForRender(EntityId e, Matrix4 model);
	void RenderTheQueue();
	std::vector<std::string> m_modelsPaths;
	std::vector<ModelData> m_models;
	Entity m_camera;
	bool m_isFirstTick = true;
	std::vector<MeshQueue> m_meshQueues;
	//std::vector<unsigned int> m_bonesTransforms;
};