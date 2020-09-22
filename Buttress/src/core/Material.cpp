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
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse->m_textureNo);
	}
	else
	{
		// do nothing
		return;
	}
	if (specular != nullptr && shader->IsAttributeDefined(UNIFORM_SAMPLER2D_SPECULAR))
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

bool MaterialData::IsReady()
{
	return (shader != nullptr && shader->IsShaderReady());
}

void MaterialData::Debug()
{
	PRINT("INFO", "material ready:", IsReady() ? "yes" : "no");
	PRINT("    ", "material name:", name);
	PRINT("    ", "shader name:", shader->name);
	PRINT("    ", "diffuse name:", diffuse != nullptr ? diffuse->m_path : "none");
	PRINT("    ", "specular name:", specular != nullptr ? specular->m_path : "none");

}

std::vector<MaterialData> MaterialLoader::m_materialCaches;

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