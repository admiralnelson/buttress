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
	eulerAngle = Vec3(0.0f, 0.0f, -90.0f );
	UpdateVectors();
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
	//PRINT("projection set");
	view = View();
	shader->SetUniformMat4x4("view", view);
	//PRINT("view set");
}

void Camera::MouseLook(Vec2 deltaPos, bool lockPitch)
{
	PRINT("deltaX", deltaPos.x, "deltaY", deltaPos.y);
	
	deltaPos.x *= sensitivity;
	deltaPos.y *= sensitivity;

	eulerAngle.y += deltaPos.x;
	eulerAngle.z += deltaPos.y;

	// make sure that when eulerAngle.y is out of bounds, screen doesn't get flipped
	if (eulerAngle.y > 89.0f)
		eulerAngle.y = 89.0f;
	if (eulerAngle.y < -89.0f)
		eulerAngle.y = -89.0f;

	UpdateVectors();
}

void Camera::Move(Direction dir, float dt)
{
	float velocity = speed * dt;
	switch (dir)
	{
	case Camera::FORWARD:
		transform.position += transform.front * velocity;
		break;
	case Camera::BACKWARD:
		transform.position -= transform.front * velocity;
		break;
	case Camera::LEFT:
		transform.position -= transform.right * velocity;
		break;
	case Camera::RIGHT:
		transform.position += transform.right * velocity;
		break;
	default:
		break;
	}
}

void Camera::MouseZoom(float dy)
{
	fov -= dy;
	fov = glm::clamp(fov, 1.0f, 100.0f);
}

void Camera::Debug()
{
	PRINT("camera:", name);
	PRINT("rotation euler:", glm::to_string(eulerAngle));
	PRINT("transform.position:", glm::to_string(transform.position));
	PRINT("transform.front:", glm::to_string(transform.front));
}

Matrix4 Camera::Projection()
{
	return glm::perspective(glm::radians(fov), (float)windowDimension.x / (float)windowDimension.y, 0.1f, 100.0f);
}

Matrix4 Camera::View()
{
	return glm::lookAt(transform.position, transform.position + transform.front, transform.up);
}

void Camera::UpdateVectors()
{
	//X = ROLL
	//Y = YAW
	//Z = PITCH
	glm::vec3 front;
	front.x = cos(glm::radians(eulerAngle.z)) * cos(glm::radians(eulerAngle.y));
	front.y = sin(glm::radians(eulerAngle.y));
	front.z = sin(glm::radians(eulerAngle.z)) * cos(glm::radians(eulerAngle.y));
	transform.front = glm::normalize(front);

	transform.right = glm::normalize(glm::cross(transform.front, transform.worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	transform.up = glm::normalize(glm::cross(transform.right, transform.front));
}
