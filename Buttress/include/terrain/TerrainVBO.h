#pragma once
#include "pch.h"
#include "glad/glad.h"
class TerrainVBO
{
public:
	//MUST BE EXECUTED IN MAIN THREAD!
	TerrainVBO()
	{
		
	}

	//MUST BE EXECUTED IN MAIN THREAD!
	void Allocate(std::vector<Vec2>& vertices, int patchSize)
	{
		if (m_isAllocated)
		{
			PRINT("WARN", "terrain vbo has allocated previously!");
			return;
		}
		glGenBuffers(1, &m_vbo);
		glGenVertexArrays(1, &m_vao);
		m_size = 0;
		m_isAllocated = false;
		m_isAllocated = true;
		m_size = vertices.size();
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, nullptr);
		glPatchParameteri(GL_PATCH_VERTICES, patchSize);	
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	//MUST BE EXECUTED IN MAIN THREAD!
	void Draw()
	{
		glBindVertexArray(m_vao);
		glDrawArrays(GL_PATCHES, 0, m_size);
		
	}
	
private:
	GLuint m_vbo;
	GLuint m_vao;
	GLuint m_size;
	bool m_isAllocated;
};