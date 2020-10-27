#pragma once
#include "pch.h"
#include "terrain\TerrainContainer.h"
#include "terrain\TerrainConfig.h"
#include "terrain\TerrainQuadtree.h"
#include "components\Transform.h"
class Terrain : public TerrainContainer
{
public:
	Terrain(TerrainConfig config, Entity camera);
	
	//called from mouse callback
	void UpdateQuadTree();
private:
	TerrainConfig m_config;
};