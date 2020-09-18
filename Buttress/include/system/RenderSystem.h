#pragma once
#include "ECS.h"
#include "components/Transform.h"
#include "components/Mesh.h"
#include "system/CameraSystem.h"
#include "core/Material.h"
#include "glad/glad.h"

class Universe;
class RenderSystem : public System
{
	friend class Universe;
public:
	void Init(Universe* universe) override;
	void Tick();
private:
	bool LoadModel(std::string objPath);
	struct Model
	{
		GLuint m_vbo = 0, m_vao = 0, m_ibo = 0;
		GLuint m_indicesSize = 0;
		std::shared_ptr<Material> material;
	};
	
	std::unordered_map<std::string, Model> m_models;
	Entity m_camera;
	bool m_isFirstTick = true;
};