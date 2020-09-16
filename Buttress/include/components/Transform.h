#include "pch.h"

struct Transform
{
	Vec3 position = { 0, 0, 0 };
	Vec3 scale = { 1.0f,1.0f,1.0f };
	Quaternion localRotation;
	Vec3 front = { 0.0f, 0.0f, -1.0f };
	Vec3 up = { 0.0f, 1.0f, 0.0f };
	Vec3 right = { 1.0f, 0.0f, 0.0f };
	const Vec3 worldUp{ 0.0f, 1.0f, 0.0f };
	Matrix4 GetTransformation();
	void Rotate(Vec3 rotation);
	void RotateDeg(Vec3 rotation);
	Vec3 RotationEulerDeg();
	Transform(Transform& t) : position(t.position), scale(t.scale), up(t.up), right(t.right), localRotation(t.localRotation) {}
};