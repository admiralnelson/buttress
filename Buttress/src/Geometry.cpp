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

glm::mat4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& from)
{
	glm::mat4 to;

	to[0][0] = (float)from.a1; to[0][1] = (float)from.b1;  to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
	to[1][0] = (float)from.a2; to[1][1] = (float)from.b2;  to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
	to[2][0] = (float)from.a3; to[2][1] = (float)from.b3;  to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
	to[3][0] = (float)from.a4; to[3][1] = (float)from.b4;  to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;

	return to;
}

