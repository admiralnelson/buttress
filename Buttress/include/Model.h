#pragma once
#include "pch.h"
#include "Texture.h"
#include "Shader.h"
class Model
{
public:
	Model(std::string name, std::shared_ptr<Shader> shader, std::string path);
	Model(std::string name, std::shared_ptr<Shader> shader, std::vector<Vertex> verts, std::vector<unsigned int> indices);
	void Draw(std::shared_ptr<Texture> texture);
private:
	std::string m_name;
	GLuint m_vbo = 0, m_vao = 0, m_ibo = 0, m_elementCounts = 0;
	std::shared_ptr<Shader> m_shader;
};