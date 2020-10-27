#pragma once
#include "pch.h"
#include "terrain\Terrain.h"

Terrain::Terrain(TerrainConfig config, Entity camera)
{
	m_config = config;
	m_config.camera = camera;
	AddChild(new TerrainQuadtree(m_config));
}

void Terrain::UpdateQuadTree()
{
	//dynamic_cast<TerrainQuadtree*>(GetChildren(0))->UpdateQuadtree();
}
