#pragma once
#include "pch.h"
#include "Material.h"
#include "MeshData.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "components/Transform.h"
#include "system/RenderSystem.h"

const unsigned int MAX_BONES = 100;
#define NUM_BONES_PER_VERTEX 4

class RenderSystem;
struct Vertex
{
	Vec3 position = { 0,0,0 };
	Vec3 normal = { 0,0,0 };
	Vec2 uv = { 0,0 };
};

struct VertexBoneData
{
	std::array<unsigned int, NUM_BONES_PER_VERTEX> ids;
	std::array<float, NUM_BONES_PER_VERTEX> weights;
	void AddBoneData(unsigned int id, float weight)
	{
		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(ids); i++) {
			if (weights[i] == 0.0) {
				ids[i] = id;
				weights[i] = weight;
				return;
			}
		}
	}
};



class MeshData;
class MeshLoader;
class ModelData
{
	friend class RenderSystem;
	friend class AnimationSystem;
public:
	ModelData() {}
	ModelData(std::string path, Entity &e);
	void Draw(Matrix4 proj, Matrix4 view, Matrix4 model);
	//only called for entity with loaded model but not animation
	void AttachAnimationToEntity(Entity& e);
	static std::shared_ptr<Shader> defaultShader;
	static std::shared_ptr<Shader> defaultAnimatedShader;
private:
	void ProcessNode(aiNode *node, const aiScene *scene, Entity &e);
	MeshId ProcessMesh(aiMesh *mesh, const aiScene *scene, Entity &e);
	MaterialData ProcessMaterial(aiMaterial *material,  std::string name);


	//auxiliary method
	void AttachAnimationRecursively(aiNode *node, const aiScene *scene, Entity& e);
	void PopulateBoneData(aiMesh* mesh, const aiScene* scene, Entity& e);

	std::vector<unsigned int> m_meshes;
	std::string m_path;
	std::shared_ptr<Shader> m_shader;
	RenderSystem* m_renderer;
	std::shared_ptr<Assimp::Importer> m_importer;
};

class ModelLoader
{
public:
	static ModelLoader& Instance();
	bool IsModelLoaded(ModelId id);
	ModelId LoadModel(std::string path, Entity &e);
	ModelData& GetModel(ModelId id);
	void ClearCache();
private:
	tbb::concurrent_vector<std::string> m_modelNames;
	tbb::concurrent_vector<ModelData> m_modelCaches;
	ModelLoader() {}
};

