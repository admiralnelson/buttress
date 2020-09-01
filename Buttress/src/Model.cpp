#include "pch.h"
#include "Model.h"

static std::unordered_map<std::string, Model*> modelList;

Model::Model(std::string name, Shader *shader, std::string path)
{
}

Model::Model(std::string name, Shader* shader, std::vector<Vertex>& verts, std::vector<unsigned int>& indices)
{
	m_name = name;
	m_shader.reset(shader);
	if (!m_shader->IsShaderReady())
	{
		PRINT("WARNING", "shader", m_shader->name, " is not ready");
		return;
	}
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
	glEnableVertexArrayAttrib(m_vao, m_shader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, m_shader->GetAttributeLocation(ATTRIBUTE_COLOR));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, m_shader->GetAttributeLocation(ATTRIBUTE_UV));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	modelList[m_name] = this;
}

void Model::Draw(std::shared_ptr<Texture> tex)
{
	if (tex != nullptr)
	{
		tex->Use();
	}
	else
	{
		if (texture != nullptr)
		{
			texture->Use();
		}
	}
	m_shader->Use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glDrawElements(GL_TRIANGLES, m_elementCounts, GL_UNSIGNED_INT, nullptr);
}

