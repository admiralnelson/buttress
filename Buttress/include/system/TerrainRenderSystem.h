#pragma once
#include "pch.h"
#include "terrain/TerrainConfig.h"
#include "terrain/Terrain.h"
#include "terrain/TerrainContainer.h"
#include "core/Shader.h"

//TODO: WARNING
//this could be game engine bottleneck, since render/update function blocks the game execution
//mitigation: refactor "patch" meshes to be queued into rendersystem and use ECS approach!
class TerrainRenderSystem
{
public:
	static TerrainRenderSystem& Instance()
	{
		static TerrainRenderSystem instance;
		return instance;
	}

	void AddTerrain(TerrainConfig config, Entity camera);
	void Render();
	void ClearTerrain();
private:
	TerrainRenderSystem() {}
	void InitShader();

	TerrainContainer* m_rootTerrain = nullptr;
	std::shared_ptr<Shader> m_terrainShader;
};