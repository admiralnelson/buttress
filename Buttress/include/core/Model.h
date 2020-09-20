#pragma once
#include "pch.h"
#include "Material.h"
#include "MeshData.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "components/Transform.h"

#define NUM_BONES_PER_VERTEX 10

struct Vertex
{
	Vec3 position = { 0,0,0 };
	Vec3 normal = { 0,0,0 };
	Vec2 uv = { 0,0 };
};

struct VertexBoneData
{
	unsigned int ids[NUM_BONES_PER_VERTEX];
	float weights[NUM_BONES_PER_VERTEX];
};

struct BoneInfo
{
	Matrix4 boneOffset = Matrix4();
	Matrix4 finalTransformation = Matrix4();
};

class MeshData;
class Model
{
	friend class RenderSystem;
public:
	Model() {}
	Model(std::string path);
	void Draw(Matrix4 proj, Matrix4 view, Matrix4 model);
	static std::shared_ptr<Shader> defaultShader;
private:
	void ProcessNode(aiNode *node, const aiScene *scene);
	MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);
	Material ProcessMaterial(aiMaterial *material,  std::string name);
	std::vector<MeshData> m_meshes;
	std::string m_path;
	std::shared_ptr<Shader> m_shader;
	std::map<std::string, unsigned int> m_boneNameToIndex;
	unsigned int m_numberOfBones = 0;

};