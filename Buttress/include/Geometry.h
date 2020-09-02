#pragma once
#include "glm/common.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

typedef glm::tvec2<float> Vec2;
typedef glm::tvec3<float> Vec3;
typedef glm::quat Quarternion;
typedef glm::mat4x4 Matrix4;

struct Vertex
{
	Vec3 position;
	Vec3 colour;
	Vec2 uv;
};

Matrix4 Rotate(Matrix4 &input, Vec3 axis);

struct Transformation
{
	Vec3 position = {0,0,0};
	Vec3 scale = {1,1,1};
	Vec3 rotation = { 0,0,0 };

	Matrix4 GetTransformation()
	{
		Matrix4 modelMatrix = Matrix4(1);
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = Rotate(modelMatrix, rotation);
		modelMatrix = glm::scale(modelMatrix, scale);
		return modelMatrix;
	}
};