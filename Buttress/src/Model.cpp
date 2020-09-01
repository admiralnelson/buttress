#include "pch.h"
#include "Model.h"

static std::unordered_map<std::string, Model*> modelList;

Model::Model(std::string name, std::string path)
{
}

Model::Model(std::string name, std::shared_ptr<Shader> shader, std::vector<Vertex>& verts, std::vector<unsigned int>& indices)
{
	this->name = name;
	this->shader = shader;
	if (!this->shader->IsShaderReady())
	{
		PRINT("WARNING", "shader", this->shader->name, " is not ready");
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
	glEnableVertexArrayAttrib(m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_COLOR));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_UV));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	modelList[this->name] = this;
}


Model::Model(std::string name, std::shared_ptr<Material> material, std::vector<Vertex>& verts, std::vector<unsigned int>& indices)
{
	this->name = name;
	this->material = material;
	if (!this->material->IsReady())
	{
		PRINT("WARNING", "material", this->material->name, " is not ready");
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
	glEnableVertexArrayAttrib(m_vao, this->material->shader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, this->material->shader->GetAttributeLocation(ATTRIBUTE_COLOR));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, this->material->shader->GetAttributeLocation(ATTRIBUTE_UV));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	modelList[this->name] = this;
}


void Model::DrawTexture(std::shared_ptr<Texture> tex)
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
	if (shader == nullptr)
	{
		PRINT("WARNING", "model", name, "has no shader assigned");
		return;
	}
	shader->Use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glDrawElements(GL_TRIANGLES, m_elementCounts, GL_UNSIGNED_INT, nullptr);
}

void Model::Draw(std::shared_ptr<Material> mat)
{
	if (mat != nullptr)
	{
		mat->Use();
	}
	else
	{
		if (material != nullptr)
		{
			material->Use();
		}
		else
		{
			PRINT("WARNING", "model", name, "has no material assigned");
			return;
		}
	}
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glDrawElements(GL_TRIANGLES, m_elementCounts, GL_UNSIGNED_INT, nullptr);
}

Model::~Model()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vbo);
}
