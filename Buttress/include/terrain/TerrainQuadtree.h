#pragma once
#include "pch.h"
#include "core/Types.h"
#include "terrain/TerrainContainer.h"
#include "terrain/TerrainConfig.h"
#include "terrain/TerrainNode.h"
#include "terrain/TerrainVBO.h"
class TerrainQuadtree : public TerrainContainer
{
public:
	TerrainQuadtree(TerrainConfig config);
	void UpdateQuadtree();

private:
	
};