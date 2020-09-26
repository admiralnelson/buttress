#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "Material.h"
#include "Shader.h"
#include "components/Transform.h"
#include "core/ModelData.h"

struct Vertex;
struct VertexBoneData;
struct BoneInfo;
class ModelData;

class MeshData
{
public:

	MeshData(std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones, MaterialId mat);
	void Draw(Matrix4& proj, Matrix4& view, Matrix4& model);
	void Use();
	MaterialId GetMaterialId() const { return m_matId; }
private:
	MaterialId m_matId;

	struct MeshHandle
	{
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		GLuint vertexBonesO;
		GLuint indexSize;
	};

	MeshHandle m_meshData;
};

class MeshLoader
{
	friend class RenderSystem;
public:
	static MeshId LoadMesh(std::string name, MeshData data);
	static MeshData& GetMesh(MeshId id);
private:
	static void ClearCache();

	static std::vector<std::string> m_meshNames;
	static std::vector<MeshData> m_meshCaches;
};

