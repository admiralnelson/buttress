#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "components/Terrain.h"
class TerrainQuadTree
{
public:
	TerrainQuadTree(Terrain& terrainConfig);
	void Draw(Matrix4& proj, Matrix4& view, Matrix4& model);
	~TerrainQuadTree();
private:
	struct TerrainMeshHandle
	{
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
	};

	TerrainMeshHandle m_terrainMeshHandle{ 0,0,0 };
};

class TerrainQuadTreeStorage
{
public:
	void StoreQuadTree(TerrainQuadTree& t);
};