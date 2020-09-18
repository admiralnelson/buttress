#pragma once
#include "pch.h"
#include "Texture.h"
#include "core\Shader.h"

class Material
{
public:
	Material() {}
	Material(std::string name, std::shared_ptr<Shader> shader);
	std::shared_ptr<TextureData> diffuse;
	std::shared_ptr<TextureData> specular;
	std::shared_ptr<Shader> shader;
	
	std::string name;
	void Use();
	bool IsReady();
	void Debug();
private:
	
};