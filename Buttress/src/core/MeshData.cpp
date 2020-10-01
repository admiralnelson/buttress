#include "pch.h"
#include "core/MeshData.h"
#include "util.h"
MeshData::MeshData(std::vector<Vertex>& verts, std::vector<unsigned int>& indices, std::vector<VertexBoneData>& bones, MaterialId mat)
{
	std::copy(verts.begin(), verts.end(), std::back_inserter(m_verts));
	std::copy(indices.begin(), indices.end(), std::back_inserter(m_indices));
	std::copy(bones.begin(), bones.end(), std::back_inserter(m_bones));
	m_matId = mat;
	PRINT("INFO", "mesh data has been loaded");
}

void MeshData::Draw(Matrix4& proj, Matrix4& view, Matrix4& model)
{	
	MaterialData& material = MaterialLoader::GetMaterialById(m_matId);
	
	//set the uniform
	Matrix4 pvm = proj * view * model;
	material.shader->SetUniformMat4x4(UNIFORM_MATRIX4_PVM, pvm);
	material.shader->SetUniformMat4x4(UNIFORM_MATRIX4_MODEL, model);
	//draw the object
	if (m_meshData.vao != 0)
	{
		glDrawElements(GL_TRIANGLES, m_meshData.indexSize, GL_UNSIGNED_INT, nullptr);
	}
	
}

void MeshData::Use()
{
	if (m_meshData.vao == 0)
	{
		MeshHandle meshData;
		MaterialData& material = MaterialLoader::GetMaterialById(m_matId);
		if (!material.shader && !material.shader->IsShaderReady())
		{
			PRINT("WARNING", "shader", material.shader->name, " is not ready");
			return;
		}
		meshData.indexSize = (GLuint)m_indices.size();
		glGenVertexArrays(1, &meshData.vao);
		glGenBuffers(1, &meshData.vbo);
		glGenBuffers(1, &meshData.ibo);
		if (m_bones.size() > 0)
		{
			glGenBuffers(1, &meshData.vertexBonesO);
		}

		glBindVertexArray(meshData.vao);

		glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verts.size(), m_verts.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexArrayAttrib(meshData.vao, material.shader->GetAttributeLocation(ATTRIBUTE_POS));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
		glEnableVertexArrayAttrib(meshData.vao, material.shader->GetAttributeLocation(ATTRIBUTE_NORMAL));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
		glEnableVertexArrayAttrib(meshData.vao, material.shader->GetAttributeLocation(ATTRIBUTE_UV));

		if (m_bones.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexBonesO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData) * m_bones.size(), m_bones.data(), GL_STATIC_DRAW);
		}

		int err = 0;
		if (m_bones.size() > 0)
		{
			err = glGetError();
			glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (void*)0);
			err = glGetError();
			glEnableVertexArrayAttrib(meshData.vao, material.shader->GetAttributeLocation(ATTRIBUTE_BONE_IDS));
			err = glGetError();

			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(NUM_BONES_PER_VERTEX * sizeof(float)));
			err = glGetError();
			glEnableVertexArrayAttrib(meshData.vao, material.shader->GetAttributeLocation(ATTRIBUTE_BONE_WEIGHTS));
			err = glGetError();

		}

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_meshData = meshData;

		PRINT("INFO", "mesh data has been created vao:", meshData.vao, "vbo:", meshData.vbo, "ibo:", meshData.ibo, "index size:", meshData.indexSize);
		err = glGetError();
		if (err != 0)
		{
			PRINT("ERROR");
		}
		m_verts.clear();
		m_indices.clear();
		m_bones.clear();
		glBindVertexArray(m_meshData.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshData.ibo);
	}
	else
	{
		glBindVertexArray(m_meshData.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshData.ibo);
	}
	
}

std::vector<MeshData> MeshLoader::m_meshCaches;
std::vector<std::string> MeshLoader::m_meshNames;


MeshId MeshLoader::LoadMesh(std::string name, std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones, MaterialId mat)
{
	name = RandomString(5);
	if (std::find(m_meshNames.begin(), m_meshNames.end(), name) == m_meshNames.end())
	{
		MeshData data(verts, indices, bones, mat);
		m_meshNames.push_back(name);
		m_meshCaches.push_back(data);
		return (MeshId)m_meshCaches.size() - 1;
	}
	else
	{
		auto it = std::find(m_meshNames.begin(), m_meshNames.end(), name);
		return (MeshId)std::distance(m_meshNames.begin(), it);
	}
}

MeshData& MeshLoader::GetMesh(MeshId id)
{
	if (id >= m_meshCaches.size())
	{
		PRINT("ERROR", "meshId out of range! id", id);
		throw std::exception("mesh out of range");
	}
	return m_meshCaches[id];
}
