#pragma once
#include "pch.h"
#include "ECS.h"
#include "core/TerrainQuadTree.h"
class TerrainSystem : public System
{
public:
	struct TerrainQueue
	{
		TerrainQuadTree terrain;
	};
	virtual void Init(Universe* universe) override;
private:
	void CreateTerrain(Entity ent, std::string path);
	void UpdateQuadtree(Entity ent);
	unsigned int UpdateMorphingArea(unsigned int level);
	void Process(size_t index);
	void Render();
	
	Entity m_camera;
	bool m_isFirstTick;
};