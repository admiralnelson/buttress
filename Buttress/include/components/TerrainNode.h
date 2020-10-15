#pragma once
#include "pch.h"
#include "core/TerrainQuadTree.h"
struct TerrainNode
{
	friend class TerrainSystem;
private:
	int	lod;
	Vec2 location;
	float gap;
	TerrainQuadId quadTreeIndex;
};