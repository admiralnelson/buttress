#include "pch.h"
#include "core/TerrainQuadTree.h"

TerrainQuadTree::TerrainQuadTree(Terrain& terrainConfig)
{
	for (size_t i = 0; i < ROOT_NODES; i++)
	{
		for (size_t j = 0; j < ROOT_NODES; j++)
		{

		}
	}
}

void TerrainQuadTreeStorage::AddQuadTree(TerrainQuadTree t)
{
	m_caches.emplace_back(t);
}

TerrainQuadTree& TerrainQuadTreeStorage::GetTerrainQuadById(TerrainQuadId id)
{
	if (id >= m_caches.size())
	{
		PRINT("ERROR", "terrainQuadId out of range! id", id);
		throw std::runtime_error("terrainQuad out of range");
	}
	return m_caches[id];
}

TerrainQuadTreeStorage& TerrainQuadTreeStorage::Instance()
{
	static TerrainQuadTreeStorage instance;
	return instance;
}
