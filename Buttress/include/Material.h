#pragma once
#include "pch.h"
#include "Texture.h"
#include "Shader.h"

class Material
{
friend class Model;
public:
	Material(std::string name, std::shared_ptr<Shader> shader);
	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> specular;
	std::shared_ptr<Shader> shader;
	
	std::string name;
	void Use();
	bool IsReady();
	void Debug();
private:
	
};