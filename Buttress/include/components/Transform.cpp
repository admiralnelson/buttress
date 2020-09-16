#include "pch.h"
#include "Transform.h"

Matrix4 Transform::GetTransformation()
{
	Matrix4 modelMatrix = Matrix4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = modelMatrix * glm::toMat4(localRotation);
	modelMatrix = glm::scale(modelMatrix, scale);
	return modelMatrix;
}

void Transform::Rotate(Vec3 rotation)
{
	localRotation = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
}

void Transform::RotateDeg(Vec3 rotation)
{
	localRotation = glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
}

Vec3 Transform::RotationEulerDeg()
{
	return Vec3{ glm::degrees(glm::eulerAngles(localRotation)) };
}
