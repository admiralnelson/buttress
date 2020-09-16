#include "pch.h"
#include "system/CameraSystem.h"

void CameraSystem::Init(Universe* universe)
{
	m_universe = universe;
}

void CameraSystem::SetupCamera(float fov, Vec2 windowDimension)
{
	this->windowDimension = windowDimension;
	this->fov = fov;
	for (auto const& i : m_entity)
	{
		Entity e = m_universe->QueryByEntityId(i);
		e.GetComponent<Transform>().RotateDeg(Vec3(0));
	}
	UpdateVectors();
}

void CameraSystem::MouseLook(Vec2 deltaPos, bool lockPitch)
{
	if (m_mainCamera.IsValid())
	{
		PRINT("deltaX", deltaPos.x, "deltaY", deltaPos.y);

		deltaPos.x *= sensitivity;
		deltaPos.y *= sensitivity;

		Vec3 rotation = m_mainCamera.GetComponent<Transform>().RotationEulerDeg();
		rotation.x += deltaPos.x;
		rotation.y += deltaPos.y;

		rotation.x = ClampAngle(rotation.x, minimumX, maximumX);
		rotation.y = ClampAngle(rotation.y, minimumY, maximumY);

		m_mainCamera.GetComponent<Transform>().RotateDeg(rotation);

		UpdateVectors();
		Debug();
	}
	else
	{
		m_mainCamera = FindPrimaryCamera();
		MouseLook(deltaPos, lockPitch);
	}
}

void CameraSystem::Move(Direction dir, float dt)
{
	if (m_mainCamera.IsValid())
	{
		Transform &transform = m_mainCamera.GetComponent<Transform>();
		float velocity = speed * dt;
		switch (dir)
		{
		case CameraSystem::FORWARD:
			transform.position += transform.front * velocity;
			break;
		case CameraSystem::BACKWARD:
			transform.position -= transform.front * velocity;
			break;
		case CameraSystem::LEFT:
			transform.position -= transform.right * velocity;
			break;
		case CameraSystem::RIGHT:
			transform.position += transform.right * velocity;
			break;
		default:
			break;
		}
	}
	else
	{
		m_mainCamera = FindPrimaryCamera();
		Move(dir, dt);
	}
}

void CameraSystem::MouseZoom(float dy)
{
	if (m_mainCamera.IsValid())
	{
		fov -= dy;
		fov = glm::clamp(fov, 1.0f, 100.0f);
	}
	else
	{
		m_mainCamera = FindPrimaryCamera();
		MouseZoom(dy);
	}
}

void CameraSystem::Debug()
{
	for (auto const& i : m_entity)
	{
		Entity e = m_universe->QueryByEntityId(i);
		std::string name = e.GetComponent<EntityName>().name;
		PRINT("camera:", name);
		PRINT("is primary?", m_mainCamera == e);
		PRINT("rotation euler:", glm::to_string(e.GetComponent<Transform>().RotationEulerDeg()));
		PRINT("transform.position:", glm::to_string(e.GetComponent<Transform>().position));
		PRINT("transform.front:", glm::to_string(e.GetComponent<Transform>().front));
	}
}

Entity CameraSystem::FindPrimaryCamera()
{
	for (auto const& i : m_entity)
	{
		Entity e = m_universe->QueryByEntityId(i);
		if (e.GetComponent<Camera>().IsPrimary)
		{
			m_mainCamera = e;
			return e;
		}
	}
	PRINT("ERROR", "no primary camera was found");
	return m_universe->QueryByEntityId(INVALID_ENTITY);
}

Matrix4 CameraSystem::Projection()
{
	return glm::perspective(glm::radians(fov), (float)windowDimension.x / (float)windowDimension.y, 0.1f, 100.0f);
}

Matrix4 CameraSystem::View()
{
	if (m_mainCamera.IsValid())
	{
		Transform& transform = m_mainCamera.GetComponent<Transform>();
		return glm::lookAt(transform.position, transform.position + transform.front, transform.up);
	}
	else
	{
		m_mainCamera = FindPrimaryCamera();
		return View();
	}
}

Matrix4 CameraSystem::View(Entity ent)
{
	Transform& transform = ent.GetComponent<Transform>();
	return glm::lookAt(transform.position, transform.position + transform.front, transform.up);
}

Matrix4 CameraSystem::Projection(Entity ent)
{
	Transform& transform = ent.GetComponent<Transform>();
	return  glm::perspective(glm::radians(fov), (float)windowDimension.x / (float)windowDimension.y, 0.1f, 100.0f);
}

void CameraSystem::UpdateVectors()
{
	for (auto const& i : m_entity)
	{
		Entity e = m_universe->QueryByEntityId(i);
		Transform& transform = e.GetComponent<Transform>();
		glm::vec3 front;
		Vec3 rotation = transform.RotationEulerDeg();
		front.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
		front.y = sin(glm::radians(rotation.y));
		front.z = sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
		transform.front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		transform.right = glm::normalize(glm::cross(transform.front, transform.worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		transform.up = glm::normalize(glm::cross(transform.right, transform.front));
	}
}
