#pragma once
#include "glm/common.hpp"

typedef glm::tvec2<float> Vec2;
typedef glm::tvec3<float> Vec3;

struct Vertex
{
	Vec3 position;
	Vec3 colour;
	Vec2 uv;
};
