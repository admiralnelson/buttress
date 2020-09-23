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
	static std::shared_ptr<Shader> defaultShader;
private:
	void ProcessNode(aiNode *node, const aiScene *scene, Entity &e);
	MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene, Entity &e);
	MaterialData ProcessMaterial(aiMaterial *material,  std::string name);
	std::vector<unsigned int> m_meshes;
	std::string m_path;
	std::shared_ptr<Shader> m_shader;
	RenderSystem* m_renderer;
	std::shared_ptr<Assimp::Importer> m_importer;
};