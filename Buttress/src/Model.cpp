#include "pch.h"
#include "Model.h"

static std::unordered_map<std::string, Model*> modelList;

Model::Model(std::string name, std::shared_ptr<Shader> shader, std::string path)
{
}

Model::Model(std::string name, std::shared_ptr<Shader> shader, std::vector<Vertex> verts, std::vector<unsigned int> indices)
{
	m_name = name;
	m_shader = shader;
	m_elementCounts = indices.size();

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexArrayAttrib(m_vao, m_shader->GetAttributeLocation("aPos"));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexArrayAttrib(m_vao, m_shader->GetAttributeLocation("aColor"));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * 2 * sizeof(float)));
	glEnableVertexArrayAttrib(m_vao, m_shader->GetAttributeLocation("aUv"));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	modelList[m_name] = this;
}

void Model::Draw(std::shared_ptr<Texture> texture)
{
	if (texture == nullptr) return;
	texture->Use();
	m_shader->Use();
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_elementCounts, GL_UNSIGNED_INT, nullptr);
}
