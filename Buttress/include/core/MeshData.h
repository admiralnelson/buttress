#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "Material.h"
#include "Shader.h"
#include "components/Transform.h"
#include "core/ModelData.h"

typedef unsigned int MeshId;
const MeshId INVALID_MESH = -1;

struct Vertex;
struct VertexBoneData;
struct BoneInfo;
class ModelData;

class MeshLoader
{
	friend class RenderSystem;
public:
	static MeshId LoadMesh(MeshData data)
	{
		m_meshCaches.push_back(data);
		return m_meshCaches.size();
	}
	static MeshData& GetMesh(MeshId id)
	{
		if (id < 0 || id > m_meshCaches.size())
		{
			PRINT("ERROR", "meshId out of range! id", id);
			throw std::exception("mesh out of range");
		}
		return m_meshCaches[id];
	}

private:
	static void ClearCache()
	{

	}

	static std::vector<MeshData> m_meshCaches;
};

class MeshData
{
public:

	MeshData(std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones, MaterialData mat);
	void Draw(Matrix4 &proj, Matrix4 &view, Matrix4& model);
private:
	MaterialData m_material;
	
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