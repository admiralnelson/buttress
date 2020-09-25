#include "pch.h"
#include "Geometry.h"
#include "glm/gtx/matrix_decompose.hpp"

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

Quaternion aiQuaternionToQuaternion(const aiQuaternion& q)
{
	 return glm::quat(q.w, q.x, q.y, q.z); 
}

Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& from)
{
	return glm::transpose(glm::make_mat4(&from.a1));
}

Matrix4 aiMatrix3x3ToMatrix4(const aiMatrix3x3& m)
{
	return glm::transpose(glm::make_mat3(&m.a1));
}

Matrix4 QuaternionToMatrix4(const Quaternion& input)
{
	return glm::toMat4(input);
}

Matrix4 ScaleToMatrix4(Vec3& v)
{
	return glm::scale(Matrix4(1), v);
}

Matrix4 TranslationToMatrix4(Vec3& v)
{
	return glm::translate(Matrix4(1), v);
}

void DecomposeMatrix4(Matrix4& mat, Vec3& outScale, Quaternion& outRot, Vec3& outTrans, Vec3& outSkew, Vec4& outPers)
{
	glm::decompose(mat, outScale, outRot, outTrans, outSkew, outPers);
}

Matrix4 Inverse(Matrix4& in)
{
	return glm::inverse(in);
}

