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
	Entity& GetChild(size_t idx) { return childs[idx]; }
	size_t TotalChild() { return childs.size(); }
private:
	std::vector<Entity> childs;
	EntityId parent = INVALID_ENTITY;
	EntityId mainParent = INVALID_ENTITY;
};