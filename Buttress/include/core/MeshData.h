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

	MeshData(std::vector<Vertex>& verts, std::vector<unsigned int>& indices, std::vector<VertexBoneData>& bones, MaterialId mat);
	//MUST BE EXECUTED ON MAIN THREAD!
	void Draw(Matrix4& proj, Matrix4& view, Matrix4& model);
	//MUST BE EXECUTED ON MAIN THREAD!
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

	std::vector<Vertex> m_verts;
	std::vector<unsigned int> m_indices;
	std::vector<VertexBoneData> m_bones;
	MeshHandle m_meshData = {0,0,0,0,0};
};

class MeshLoader
{
	friend class RenderSystem;
public:
	static MeshLoader& Instance();
	MeshId LoadMesh(std::string name, std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones, MaterialId mat);
	MeshData& GetMesh(MeshId id);
private:
	MeshLoader() {}
	void ClearCache();

	tbb::concurrent_vector<std::string> m_meshNames;
	tbb::concurrent_vector<MeshData> m_meshCaches;
};

