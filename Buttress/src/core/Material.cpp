#include "pch.h"
#include "core\Material.h"

MaterialData::MaterialData(std::string name, std::shared_ptr<Shader> shader)
{
	this->name = name;
	this->shader = shader;
}

void MaterialData::Use()
{
	if (diffuse != nullptr && shader->IsUniformDefined(UNIFORM_SAMPLER2D_DIFFUSE))
	{
		shader->SetUniformValueI(UNIFORM_SAMPLER2D_DIFFUSE, 0);
		diffuse->Use();
		
	}
	else
	{
		// do nothing
		return;
	}
	if (specular != nullptr && shader->IsAttributeDefined(UNIFORM_SAMPLER2D_SPECULAR))
	{
		
		shader->SetUniformValueI(UNIFORM_SAMPLER2D_SPECULAR, 1);
		specular->Use(1);
		
	}
	else
	{
		//PRINT("WARNING", "material", name, "specular is not assigned!");
	}
}

bool MaterialData::IsReady()
{
	return (shader != nullptr && shader->IsShaderReady() && diffuse->IsReady());
}

void MaterialData::Debug()
{
	PRINT("INFO", "material ready:", IsReady() ? "yes" : "no");
	PRINT("    ", "material name:", name);
	PRINT("    ", "shader name:", shader->name);
	

}


MaterialLoader& MaterialLoader::Instance()
{
	static MaterialLoader loader;
	return loader;
}

bool MaterialLoader::IsMaterialExist(MaterialId id)
{
	return id < m_materialCaches.size();
}

MaterialId MaterialLoader::LoadMaterial(MaterialData mat)
{
	auto it = std::find(m_materialCaches.begin(), m_materialCaches.end(), mat);
	if (it != m_materialCaches.end())
	{
		return (MaterialId)std::distance(m_materialCaches.begin(), it);
	}
	PRINT("loaded material:", mat.name);
	m_materialCaches.push_back(mat);
	return (MaterialId)m_materialCaches.size() - 1;
}

MaterialData& MaterialLoader::GetMaterialById(MaterialId id)
{
	return m_materialCaches[id];
}