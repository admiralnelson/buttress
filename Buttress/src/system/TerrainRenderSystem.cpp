#pragma once
#include "pch.h"
#include "system/TerrainRenderSystem.h"

void TerrainRenderSystem::AddTerrain(TerrainConfig config, Entity camera)
{
	if (m_rootTerrain)
	{
		PRINT("WARN", "terrain already exist");
		return;
	}
	m_rootTerrain = new Terrain(config, camera);
}

void TerrainRenderSystem::Render()
{
}

void TerrainRenderSystem::ClearTerrain()
{
}
