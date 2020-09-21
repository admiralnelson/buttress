#pragma once
#include "pch.h"
#include "Texture.h"
#include "core\Shader.h"

typedef unsigned int MaterialId;
const MaterialId INVALID_MATERIAL = -1;

class MaterialData
{
public:
	MaterialData() {}
	MaterialData(std::string name, std::shared_ptr<Shader> shader);
	std::shared_ptr<TextureData> diffuse;
	std::shared_ptr<TextureData> specular;
	std::shared_ptr<Shader> shader;

	std::string name;
	void Use();
	bool IsReady();
	void Debug();
private:
	
};

inline bool operator==(const MaterialData& a, const MaterialData& b) 
{ 
	return a.diffuse == b.diffuse && a.specular == b.specular;
}