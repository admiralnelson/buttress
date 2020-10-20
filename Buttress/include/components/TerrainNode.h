#pragma once
#include "pch.h"
struct TerrainNode
{
	friend class TerrainSystem;
private:
	int	lod;
	Vec2 location;
	float gap;
	TerrainDataId terrainLodPatch;
};