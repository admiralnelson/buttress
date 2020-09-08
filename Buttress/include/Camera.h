#pragma once
#include "pch.h"
#include "Shader.h"
class Camera
{
public:
	enum Direction 
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	Camera(std::string name, std::shared_ptr<Shader> shader, float fov, Vec2 windowDimension, Transformation transform = Transformation());
	~Camera();
	void Use();
	void MouseLook(Vec2 deltaPos, bool lockPitch = true);
	void Move(Direction dir, float dt);
	void MouseZoom(float dy);
	void Debug();
	Matrix4 Projection();
	Matrix4 View();
	Vec2 windowDimension = { 0, 0 };
	Transformation transform;
	std::shared_ptr<Shader> shader;
	std::string name;
	float sensitivity = 0.1;
	float speed = 2.5;
	float fov = 45;
private:
	void UpdateVectors();
	static float ClampAngle(float angle, float min, float max)
	{
		if (angle < -360.0f)
			angle += 360.0f;
		if (angle > 360.0f)
			angle -= 360.f;
		return glm::clamp(angle, min, max);
	}
	
	float minimumX = -360.0f;
	float maximumX = 360.0f;

	float minimumY = -60.0f;
	float maximumY = 60.0f;
	Quaternion originalRotation;
};
