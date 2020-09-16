#pragma once
#include "pch.h" 
#include "ecs\SystemManager.h"
#include "ecs\ECS.h"
#include "components\Camera.h"
#include "components\Transform.h"
class CameraSystem : public System
{
public:
	enum Direction
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};
	void Init(Universe* universe) override;
	void SetupCamera(float fov, Vec2 windowDimension);
	void MouseLook(Vec2 deltaPos, bool lockPitch = true);
	void Move(Direction dir, float dt);
	void MouseZoom(float dy);
	void Debug();
	Entity FindPrimaryCamera();
	Matrix4 Projection();
	Matrix4 View();
	Matrix4 View(Entity ent);
	Matrix4 Projection(Entity ent);
	Vec2 windowDimension = { 0, 0 };
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
	Entity m_mainCamera;
};
