#pragma once
#include "core/Types.h"
#include "terrain/TerrainContainer.h"
#include "terrain/TerrainVBO.h"
#include "terrain/TerrainQuadtree.h"
#include "terrain/TerrainConfig.h"
class TerrainNode : public TerrainContainer
{
public:
	TerrainNode(TerrainVBO vbo, TerrainConfig config, Vec2 location, int lod, Vec2 index);

	void Render() override;

	void UpdateQuadtree();

	void UpdateChildNodes();

	void Add4Childs(int lod);

	void RemoveChildNodes();

	void ComputeWorldPos();
	
	TerrainContainer* GetQuadtreeParent()
	{
		return parent;
	}
	
private:
	TerrainConfig m_config;
	bool m_isLeaf;
	int m_lod;
	Vec2 m_index;
	Vec2 m_location;
	Vec3 m_worldPos;
	float m_gap;
};
