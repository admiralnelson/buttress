#include "pch.h"
#include "core/Model.h"

Model::Model(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		PRINT("ERROR", "unable to load file", path);
		PRINT("     ", "reason:", importer.GetErrorString());
		return;
	}

	if (m_path.find("/") != std::string::npos)
	{
		m_path = path.substr(0, path.find_last_of("/"));
	}
	else if (m_path.find("\\") != std::string::npos)
	{
		m_path = path.substr(0, path.find_last_of("\\"));
	}
	ProcessNode(scene->mRootNode, scene);
}

void Model::Draw()
{
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{

	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material material;

	//vertices
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		if (mesh->HasNormals())
		{
			vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		}
		if (mesh->mTextureCoords[0])
		{
			vertex.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			//vertex tangents
			//vertex bitangents

		}
		vertices.push_back(vertex);
	}

	//indices
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//materials
	aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
	material = ProcessMaterial(aimaterial, aimaterial->GetName().C_Str());
	return Mesh(vertices, indices, material);
}

Material Model::ProcessMaterial(aiMaterial* material, std::string name)
{
	//load diffuse
	std::shared_ptr<TextureData> diffuse;
	aiString path;
	material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
	diffuse = TextureLoader::Instance().LoadTexture(path.C_Str());

	//load specular
	std::shared_ptr<TextureData> specular;
	material->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &path);
	specular = TextureLoader::Instance().LoadTexture(path.C_Str());

	//load specular
	std::shared_ptr<TextureData> normalMaps;
	material->GetTexture(aiTextureType::aiTextureType_HEIGHT, 0, &path);
	normalMaps = TextureLoader::Instance().LoadTexture(path.C_Str());

	Material out;
	out.diffuse = diffuse;
	out.specular = specular;
	//out.normalmap = ...
	out.name = material->GetName().C_Str();
	out.shader = m_shader;

	

	return out;
}
