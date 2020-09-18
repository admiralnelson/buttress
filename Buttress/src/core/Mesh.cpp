#include "pch.h"
#include "core/Mesh.h"

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indices, Material mat)
{
	MeshData meshData;
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

	glBindVertexArray(meshData.vao);

	glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexArrayAttrib(meshData.vao, m_material.shader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(meshData.vao, m_material.shader->GetAttributeLocation(ATTRIBUTE_NORMAL));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(meshData.vao, m_material.shader->GetAttributeLocation(ATTRIBUTE_UV));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_meshData = meshData;
}

void Mesh::Draw()
{
}
