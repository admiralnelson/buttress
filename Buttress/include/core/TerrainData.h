#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "components/Terrain.h"
class TerrainPatch
{
	friend class TerrainPatchLoader;
public:
	void Draw(Matrix4& proj, Matrix4& view, Matrix4& model);
	~TerrainPatch();
	TerrainPatch();
private:
	struct TerrainMeshHandle
	{
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
	};

	TerrainMeshHandle m_terrainMeshHandle{ 0,0,0 };
	std::vector<Vec2> m_vertices;
	bool m_isReady = 0;
};

class TerrainPatchLoader
{
public:
	unsigned int Create(unsigned int level = 8);
	TerrainPatch& GetTerrainPatchByLodLevel(unsigned int level);
	static TerrainPatchLoader& Instance()
	{
		static TerrainPatchLoader instance;
		return instance;
	}
private:
	TerrainPatchLoader() {}
	tbb::concurrent_unordered_map<unsigned int, TerrainPatch, std::hash<unsigned int>> m_terrainPatchCaches;
};