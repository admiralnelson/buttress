#include "pch.h"
#include "Geometry.h"

Matrix4 Rotate(Matrix4 &input, Vec3 rotation)
{
	input = glm::rotate(input, rotation.x, Vec3(1, 0, 0)); //rotate X
	input = glm::rotate(input, rotation.y, Vec3(0, 1, 0)); //rotate Y
	input = glm::rotate(input, rotation.z, Vec3(0, 0, 1)); //rotate z
	return input;
}

Vec3 Quaternion2RotationEuler(Quaternion& input)
{
	return Vec3{ glm::eulerAngles(input) };
}

Vec3 Quaternion2RotationEulerDeg(Quaternion& input)
{
	return Vec3{ glm::degrees(glm::eulerAngles(input)) };
}

