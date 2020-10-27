#pragma once
#include "pch.h"
#include "system/TerrainRenderSystem.h"
#include "core/Shader.h"
#include "Util.h"
void TerrainRenderSystem::AddTerrain(TerrainConfig config, Entity camera)
{
	InitShader();
	if (m_rootTerrain)
	{
		PRINT("WARN", "terrain already exist");
		return;
	}
	m_rootTerrain = new Terrain(config, camera);
}

void TerrainRenderSystem::Render()
{
	if (!m_rootTerrain) return;
	if (!m_terrainShader) return;
	m_terrainShader->Use();
	if (m_rootTerrain)
	{
		m_rootTerrain->Render();
	}
}

void TerrainRenderSystem::ClearTerrain()
{
}

void TerrainRenderSystem::InitShader()
{
	if (!m_terrainShader)
	{
		m_terrainShader.reset(new Shader("terrainShader"));
		m_terrainShader->AddVertexShader(ReadFileAsString("../../resource/shader/terrain_vs.txt"));
		m_terrainShader->AddTesselationControlShader(ReadFileAsString("../../resource/shader/terrain_tc.txt"));
		m_terrainShader->AddTesselationEvaluationShader(ReadFileAsString("../../resource/shader/terrain_te.txt"));
		m_terrainShader->AddGeometryShader(ReadFileAsString("../../resource/shader/terrain_gs.txt"));
		m_terrainShader->AddFragmentShader(ReadFileAsString("../../resource/shader/terrain_fs.txt"));
		m_terrainShader->CompileShader();
		m_terrainShader->Validate();
		m_terrainShader->Debug();
	}
}
