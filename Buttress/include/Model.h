#pragma once
#include "pch.h"
#include "Texture.h"
#include "Shader.h"
class Model
{
public:
	Model(std::string name, Shader *shader, std::string path);
	Model(std::string name, Shader *shader, std::vector<Vertex> &verts, std::vector<unsigned int> &indices);
	void Draw(std::shared_ptr<Texture> tex = nullptr);
	std::shared_ptr<Texture> texture;
private:
	std::string m_name;
	std::shared_ptr<Shader> m_shader;
	GLuint m_vbo = 0, m_vao = 0, m_ibo = 0, m_elementCounts = 0;
};