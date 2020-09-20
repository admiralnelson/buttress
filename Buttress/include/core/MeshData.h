#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "Material.h"
#include "Shader.h"
#include "components/Transform.h"
#include "core/Model.h"

struct Vertex;
struct VertexBoneData;
struct BoneInfo;
class Model;
class MeshData
{
public:

	MeshData(std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones, Material mat);
	void Draw(Matrix4 &proj, Matrix4 &view, Matrix4& model);
private:
	Material m_material;
	
	struct MeshHandle
	{
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		GLuint indexSize;
	};

	MeshHandle m_meshData;
};