#include "pch.h"
#include "Material.h"

Material::Material(std::string name, std::shared_ptr<Shader> shader)
{
	this->name = name;
	this->shader = shader;
}

void Material::Use()
{
	if (shader != nullptr)
	{
		shader->Use();
	}
	else
	{
		PRINT("WARNING", "material", name, "shader is not assigned!");
		return;
	}
	if (diffuse != nullptr)
	{
		shader->SetUniformValueI(UNIFORM_SAMPLER2D_DIFFUSE, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse->m_textureNo);
	}
	else
	{
		PRINT("WARNING", "material", name, "diffuse is not assigned!");
		return;
	}
	if (specular != nullptr)
	{
		shader->SetUniformValueI(UNIFORM_SAMPLER2D_SPECULAR, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular->m_textureNo);
	}
	else
	{
		//PRINT("WARNING", "material", name, "specular is not assigned!");
	}
}

bool Material::IsReady()
{
	return (shader != nullptr && shader->IsShaderReady() && diffuse != nullptr);
}
