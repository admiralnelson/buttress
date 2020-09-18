#pragma once
#include "pch.h"
#include "Material.h"
#include "MeshData.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "components/Transform.h"
class Model
{
	friend class RenderSystem;
public:
	Model() {}
	Model(std::string path);
	void Draw(Matrix4 proj, Matrix4 view, Transform t);
	static std::shared_ptr<Shader> defaultShader;
private:
	void ProcessNode(aiNode *node, const aiScene *scene);
	MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);
	Material ProcessMaterial(aiMaterial *material,  std::string name);
	std::vector<MeshData> m_meshes;
	std::string m_path;
	std::shared_ptr<Shader> m_shader;
	static std::unordered_map<std::string, Model> m_cachedModels;

};