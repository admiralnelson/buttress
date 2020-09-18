#pragma once
#include "pch.h"

struct Transform
{
	Quaternion localRotation ;
	Vec3 scale = { 1.0f,1.0f,1.0f };
	Vec3 position = { 0, 0, 0 };
	Vec3 front = { 0.0f, 0.0f, -1.0f };
	Vec3 up = { 0.0f, 1.0f, 0.0f };
	Vec3 right = { 1.0f, 0.0f, 0.0f };
	const Vec3 worldUp;
	Matrix4 GetTransform();
	void Rotate(Vec3 rotation);
	void RotateDeg(Vec3 rotation);
	Vec3 RotationEulerDeg();
	Transform() : worldUp({ 0.0f, 1.0f, 0.0f }), localRotation(Quaternion()) { }
	Transform(const Transform& t) : position(t.position), scale(t.scale), up(t.up), right(t.right), localRotation(t.localRotation), worldUp({ 0.0f, 1.0f, 0.0f }) {}
	Transform operator=(const Transform& t)
	{
		Transform out;
		out.scale = t.scale;
		out.position = t.position;
		out.front = t.front;
		out.up = t.up;
		out.right = t.right;
		return out;
	}
};