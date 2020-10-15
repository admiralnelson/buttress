#pragma once
#include "pch.h"
#include "core/TerrainQuadTree.h"
struct TerrainNode
{
private:
	int	lod;
	Vec2 location;
	float gap;
	unsigned int quadTreeIndex;
};