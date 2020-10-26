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
	TerrainQuadtree(TerrainConfig config)
	{
		
		TerrainVBO terrainVbo;
		std::vector<Vec2> vertices;
		vertices.resize(16);
		int index = 0;

		vertices[index++] = Vec2(0, 0);
		vertices[index++] = Vec2(0.333f, 0);
		vertices[index++] = Vec2(0.666f, 0);
		vertices[index++] = Vec2(1, 0);

		vertices[index++] = Vec2(0, 0.333f);
		vertices[index++] = Vec2(0.333f, 0.333f);
		vertices[index++] = Vec2(0.666f, 0.333f);
		vertices[index++] = Vec2(1, 0.333f);

		vertices[index++] = Vec2(0, 0.666f);
		vertices[index++] = Vec2(0.333f, 0.666f);
		vertices[index++] = Vec2(0.666f, 0.666f);
		vertices[index++] = Vec2(1, 0.666f);

		vertices[index++] = Vec2(0, 1);
		vertices[index++] = Vec2(0.333f, 1);
		vertices[index++] = Vec2(0.666f, 1);
		vertices[index++] = Vec2(1, 1);

		terrainVbo.Allocate(vertices, 16);

		for (size_t i = 0; i < MAX_ROOT_NODES; i++)
		{
			for (size_t j = 0; j < MAX_ROOT_NODES; j++)
			{
				AddChild(new TerrainNode(terrainVbo, config, Vec2(i / (float)MAX_ROOT_NODES, j / (float)MAX_ROOT_NODES), 0, Vec2(i, j)));
			}
		}
	}

	void UpdateQuadtree()
	{
		for (auto node: nodes)
		{
			dynamic_cast<TerrainNode*>(node)->UpdateQuadtree();
		}
	}
private:
	
};