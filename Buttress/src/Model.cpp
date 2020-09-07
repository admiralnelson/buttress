#include "pch.h"
#include "Model.h"
#include "OBJ_Loader.h"

static std::unordered_map<std::string, Model*> modelList;

Model::Model(std::string name, std::shared_ptr<Shader> shader, std::string path)
{
	this->name = name;
	this->shader = shader;
	objl::Loader load;
	if (!this->shader->IsShaderReady())
	{
		PRINT("WARNING", "shader", this->shader->name, " is not ready");
		return;
	}

	if (!load.LoadFile(path))
	{
		PRINT("WARNING", "model", name, "cannot load file", path);
		return;
	}

	//TODO: LOAD MULTIPLE MATERIALS FROM FILE!

	for (size_t i = 0; i < load.LoadedMeshes.size(); i++)
	{
		ModelHandle handle;
		m_elementCounts = load.LoadedMeshes[i].Indices.size();

		glGenVertexArrays(1, &handle.m_vao);
		glGenBuffers(1, &handle.m_vbo);
		glGenBuffers(1, &handle.m_ibo);

		glBindVertexArray(handle.m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, handle.m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * load.LoadedMeshes[i].Vertices.size(), load.LoadedMeshes[i].Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * load.LoadedMeshes[i].Indices.size(), load.LoadedMeshes[i].Indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexArrayAttrib(handle.m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_POS));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
		glEnableVertexArrayAttrib(handle.m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_COLOR));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
		glEnableVertexArrayAttrib(handle.m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_UV));

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_handle.push_back(handle);
	}
	modelList[this->name] = this;
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
	ModelHandle handle;
	glGenVertexArrays(1, &handle.m_vao);
	glGenBuffers(1, &handle.m_vbo);
	glGenBuffers(1, &handle.m_ibo);

	glBindVertexArray(handle.m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, handle.m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexArrayAttrib(handle.m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(handle.m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_COLOR));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(handle.m_vao, this->shader->GetAttributeLocation(ATTRIBUTE_UV));

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
	ModelHandle handle;
	glGenVertexArrays(1, &handle.m_vao);
	glGenBuffers(1, &handle.m_vbo);
	glGenBuffers(1, &handle.m_ibo);

	glBindVertexArray(handle.m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, handle.m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexArrayAttrib(handle.m_vao, this->material->shader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(handle.m_vao, this->material->shader->GetAttributeLocation(ATTRIBUTE_COLOR));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(handle.m_vao, this->material->shader->GetAttributeLocation(ATTRIBUTE_UV));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	modelList[this->name] = this;
	m_handle.push_back(handle);
}


void Model::DrawTexture(std::shared_ptr<Texture> tex)
{
	for (ModelHandle& handle : m_handle)
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
		glBindVertexArray(handle.m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.m_ibo);
		glDrawElements(GL_TRIANGLES, m_elementCounts, GL_UNSIGNED_INT, nullptr);
	}
}

void Model::Draw(std::shared_ptr<Material> mat)
{
	for (ModelHandle& handle : m_handle)
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
		glBindVertexArray(handle.m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.m_ibo);
		glDrawElements(GL_TRIANGLES, m_elementCounts, GL_UNSIGNED_INT, nullptr);
	}
}

Model::~Model()
{
	for (ModelHandle& handle : m_handle)
	{
		glDeleteVertexArrays(1, &handle.m_vao);
		glDeleteBuffers(1, &handle.m_ibo);
		glDeleteBuffers(1, &handle.m_vbo);
	}
}
