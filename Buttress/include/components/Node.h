#pragma once
#include "pch.h"
#include "ECS.h"
class Entity;
struct Node
{
	friend class RenderSystem;
	friend class Universe;
	friend class Entity;
	bool IsLeaf()
	{
		return childs.size() == 0 && parent != INVALID_ENTITY;
	}
private:
	std::vector<Entity> childs;
	EntityId parent = INVALID_ENTITY;
	EntityId mainParent = INVALID_ENTITY;
};