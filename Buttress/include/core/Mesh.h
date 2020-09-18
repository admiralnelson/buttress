#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "Material.h"
#include "Shader.h"
typedef unsigned int MeshId;

class Mesh
{
public:
	Mesh() {}
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indices, Material mat);
	void Draw();
private:
	Material m_material;
	
	struct MeshData
	{
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		GLuint indexSize;
	};

	MeshData m_meshData;
};