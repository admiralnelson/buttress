#pragma once
#include "pch.h"
#include "Geometry.h"
#include <glad/glad.h>
#include "core/Material.h"

struct Model
{
	friend class RenderSystem;
	friend class AnimationSystem;
	std::string objectPath;
	std::unordered_map<std::string, std::shared_ptr<MaterialData>> meshMaterialOverrides;
private:
	ModelId id = INVALID_MODEL;
};