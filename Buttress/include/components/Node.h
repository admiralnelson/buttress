#pragma once
#include "pch.h"
#include "ECS.h"
class Entity;
struct Node
{
	friend class RenderSystem;
	friend class Universe;
	friend class Entity;
private:
	std::vector<Entity> childs;
	EntityId parent = INVALID_ENTITY;
};