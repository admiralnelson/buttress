#include "pch.h"
#include "core/MeshData.h"

MeshData::MeshData(std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<VertexBoneData> bones, Material mat)
{
	MeshHandle meshData;
	m_material = mat;
	if (!m_material.shader && !m_material.shader->IsShaderReady())
	{
		PRINT("WARNING", "shader", m_material.shader->name, " is not ready");
		return;
	}
	meshData.indexSize = indices.size();
	glGenVertexArrays(1, &meshData.vao);
	glGenBuffers(1, &meshData.vbo);
	glGenBuffers(1, &meshData.ibo);
	if (bones.size() > 0)
	{
		glGenBuffers(1, &meshData.vertexBonesO);
	}

	glBindVertexArray(meshData.vao);

	glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	if (bones.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexBonesO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), bones.data(), GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexArrayAttrib(meshData.vao, m_material.shader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(meshData.vao, m_material.shader->GetAttributeLocation(ATTRIBUTE_NORMAL));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(meshData.vao, m_material.shader->GetAttributeLocation(ATTRIBUTE_UV));
	
	if (bones.size() > 0)
	{
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (void*)0);
		glEnableVertexArrayAttrib(meshData.vertexBonesO, m_material.shader->GetAttributeLocation(ATTRIBUTE_BONE_IDS));

		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(NUM_BONES_PER_VERTEX * sizeof(float)));
		glEnableVertexArrayAttrib(meshData.vertexBonesO, m_material.shader->GetAttributeLocation(ATTRIBUTE_BONE_WEIGHTS));

	}
	//for animation?
	//glVertexAttribIPointer()

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_meshData = meshData;
	PRINT("INFO", "mesh data has been created vao:", meshData.vao, "vbo:", meshData.vbo, "ibo:", meshData.ibo, "index size:", meshData.indexSize);
}

void MeshData::Draw(Matrix4& proj, Matrix4& view, Matrix4& model)
{
	//TODO: SORT BY MATERIAL TO REDUCE STATE TRANSITION!
	//set the texture & use the shader
	m_material.Use();
	//set the uniform
	m_material.shader->SetUniformMat4x4("projection", proj);
	m_material.shader->SetUniformMat4x4("view", view);
	m_material.shader->SetUniformMat4x4("model", model);
	//draw the object
	glBindVertexArray(m_meshData.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshData.ibo);
	glDrawElements(GL_TRIANGLES, m_meshData.indexSize, GL_UNSIGNED_INT, nullptr);
	
}
