#pragma once
#include "ECS.h"
#include "components/Transform.h"
#include "components/Mesh.h"
#include "system/CameraSystem.h"
#include "core/Material.h"
#include "core/Model.h"
#include "glad/glad.h"
#include "components/Transform.h"
class Universe;
class RenderSystem : public System
{
	friend class Universe;
public:
	void Init(Universe* universe) override;
	void Tick();
private:
	bool RenderModel(std::string objPath, Transform entityTransform);
	
	std::unordered_map<std::string, Model> m_models;
	Entity m_camera;
	bool m_isFirstTick = true;
};