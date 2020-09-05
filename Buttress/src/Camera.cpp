#include "pch.h"
#include "Camera.h"


Camera::Camera(std::string name, std::shared_ptr<Shader> shader, float fov, Vec2 windowDimension, Transformation transform)
{
	this->name = name;
	this->transform = transform;
	this->windowDimension = windowDimension;
	if (shader)
	{
		this->shader = shader;
	}
	else
	{
		PRINT("ERROR", "camera", name, "has no assigned shader");
	}
	this->fov = fov;
}

Camera::~Camera()
{
}

void Camera::Use()
{
	shader->Use();
	Matrix4 proj, view;
	proj = Projection();
	shader->SetUniformMat4x4("projection", proj);
	PRINT("projection set");
	view = View();
	shader->SetUniformMat4x4("view", view);
	PRINT("view set");
}

Matrix4 Camera::Projection()
{
	return glm::perspective(glm::radians(fov), (float)windowDimension.x / (float)windowDimension.y, 0.1f, 100.0f);
}

Matrix4 Camera::View()
{
	Vec3 cameraFront = Vec3(0.0f, 0.0f, -1.0f);
	Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f);
	return glm::lookAt(transform.position, transform.position + cameraFront, cameraUp);
}
