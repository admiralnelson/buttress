#include "pch.h"
#include "core/TerrainData.h"

unsigned int TerrainPatchLoader::Create(unsigned int level)
{
	if (m_terrainPatchCaches.find(level) != m_terrainPatchCaches.end())
	{
		m_terrainPatchCaches[level] = TerrainPatch();
	}
	return level;
}

TerrainPatch& TerrainPatchLoader::GetTerrainPatchByLodLevel(unsigned int level)
{
	return m_terrainPatchCaches[level];
}

void TerrainPatch::Draw(Matrix4& proj, Matrix4& view, Matrix4& model)
{
	if (!m_isReady && m_vertices.size() > 0)
	{
		//load to gpu

		//clear from RAM
		m_vertices.clear();
	}
	//then draw
}

TerrainPatch::~TerrainPatch()
{
}

TerrainPatch::TerrainPatch() 
{
	m_vertices.resize(16);

	int index = 0;

	m_vertices[index++] = Vec2(0, 0);
	m_vertices[index++] = Vec2(0.333f, 0);
	m_vertices[index++] = Vec2(0.666f, 0);
	m_vertices[index++] = Vec2(1, 0);

	m_vertices[index++] = Vec2(0, 0.333f);
	m_vertices[index++] = Vec2(0.333f, 0.333f);
	m_vertices[index++] = Vec2(0.666f, 0.333f);
	m_vertices[index++] = Vec2(1, 0.333f);

	m_vertices[index++] = Vec2(0, 0.666f);
	m_vertices[index++] = Vec2(0.333f, 0.666f);
	m_vertices[index++] = Vec2(0.666f, 0.666f);
	m_vertices[index++] = Vec2(1, 0.666f);

	m_vertices[index++] = Vec2(0, 1);
	m_vertices[index++] = Vec2(0.333f, 1);
	m_vertices[index++] = Vec2(0.666f, 1);
	m_vertices[index++] = Vec2(1, 1);

}
