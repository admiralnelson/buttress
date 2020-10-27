#include "pch.h"
#include "terrain/TerrainContainer.h"

void TerrainContainer::Update()
{
	for (const auto p : nodes)
	{
		p->Update();
	}
}

void TerrainContainer::Render()
{
	for (const auto p : nodes)
	{
		p->Render();
	}
}

void TerrainContainer::AddChild(TerrainContainer* child)
{
	child->parent = this;
	nodes.emplace_back(child);
}