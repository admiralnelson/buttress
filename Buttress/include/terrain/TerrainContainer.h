#pragma once
#include "pch.h"
#include "core/Types.h"
#include "components/Transform.h"
#include "terrain/TerrainVBO.h"
class TerrainContainer
{
public:

	Transform transform;

	virtual void Update()
	{
		for (const auto p : nodes)
		{
			p->Update();
		}
	}

	virtual void Render()
	{
		for (const auto p : nodes)
		{
			p->Render();
		}
	}

	void AddChild(TerrainContainer* child)
	{
		nodes.emplace_back(child);
	}

	TerrainContainer* GetChildren(int idx)
	{
		return nodes[idx];
	}

protected:
	TerrainContainer* parent;
	std::vector<TerrainContainer*> nodes;
	TerrainVBO m_vbo;
};