#pragma once
#include "pch.h"
#include "ECS.h"
#include "core/TerrainData.h"
#include "components/TerrainNode.h"
class TerrainSystem : public System
{
public:
	struct TerrainQueue
	{
		TerrainNode terrain;
	};
	virtual void Init(Universe* universe) override;
	void Tick();
private:
	void CreateTerrain(Entity ent, std::string path);
	void CreateQuadtree(Entity parent, Terrain& terrainConfig);
	void AddTerrainNode(Entity entity, const Terrain& terrainConfig, const Vec2& location, unsigned int lod, unsigned int terrainLod);
	void Render();
	unsigned int UpdateMorphingArea(unsigned int level);
	void Process(size_t index);
	void Process(Entity ent);
	
	Entity m_camera;
	bool m_isFirstTick;
	tbb::concurrent_queue<TerrainQueue> m_terrainRenderQueue;
	std::queue<TerrainQueue> m_sortedRenderQueue;
};