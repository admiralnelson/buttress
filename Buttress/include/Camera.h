#pragma once
#include "pch.h"
#include "Shader.h"
class Camera
{
public:
	Camera(std::string name, std::shared_ptr<Shader> shader, float fov, Vec2 windowDimension, Transformation transform = Transformation());
	~Camera();
	void Use();
	Matrix4 Projection();
	Matrix4 View();
	Vec2 windowDimension = { 0, 0 };
	Transformation transform;
	std::shared_ptr<Shader> shader;
	std::string name;
	float fov;
private:
	
};
