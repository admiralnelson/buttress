#pragma once
#include "pch.h"
#include "Geometry.h"
#include <glad/glad.h>
#include "core/Material.h"

struct Mesh
{
	friend class RenderSystem;
	std::string objectPath;
	std::unordered_map<std::string, std::shared_ptr<MaterialData>> meshMaterialOverrides;

};