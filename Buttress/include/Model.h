#pragma once
#include "pch.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"

class Model
{
public:
	Model(std::string name, std::shared_ptr<Shader> shader, std::string path);
	Model(std::string name, std::shared_ptr<Shader> shader, std::vector<Vertex> &verts, std::vector<unsigned int> &indices);
	Model(std::string name, std::shared_ptr<Material> material, std::vector<Vertex>& verts, std::vector<unsigned int>& indices);
	void DrawTexture(std::shared_ptr<Texture> tex = nullptr);
	void Draw(std::shared_ptr<Material> mat = nullptr);
	std::shared_ptr<Material> material;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Shader> shader;
	std::string name;
	~Model();
private:
	struct ModelHandle
	{
		GLuint m_vbo = 0, m_vao = 0, m_ibo = 0;
	};
	std::vector<ModelHandle> m_handle;
	GLsizei m_elementCounts = 0;
};