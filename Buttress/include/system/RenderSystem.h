#pragma once
#include "ecs/ECS.h"
#include "components/Transform.h"
#include "components/Mesh.h"
#include "system/CameraSystem.h"
#include "core/Material.h"
#include "core/ModelData.h"
#include "core/MeshData.h"
#include "glad/glad.h"
#include "components/Transform.h"
class Universe;

struct MeshQueue
{
	MeshId meshId;
	Matrix4 view;
	Matrix4 model;
	Matrix4 projection;
};

class ModelData;
//class MeshLoader;
//class MaterialLoader;
class MaterialData;
class RenderSystem : public System
{
	friend class Universe;
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
	std::unordered_map <std::shared_ptr<Shader>, std::unordered_map<MaterialId, std::deque<MeshQueue>>> m_renderqueues;
	std::vector<MaterialData> m_materials;
};