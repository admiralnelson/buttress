#pragma once
#include "pch.h"
#include "core/Types.h"
#include "components/Transform.h"
#include "terrain/TerrainVBO.h"
class TerrainContainer
{
public:

	Transform localTransform;
	Transform worldTransform;

	virtual void Update();

	virtual void Render();

	void AddChild(TerrainContainer* child);

	TerrainContainer* GetChildren(int idx) { return nodes[idx]; }

protected:
	TerrainContainer* parent;
	std::vector<TerrainContainer*> nodes;
	TerrainVBO m_vbo;
};