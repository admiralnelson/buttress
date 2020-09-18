#include "pch.h"
#include "core/Model.h"

std::shared_ptr<Shader> Model::defaultShader;
Model::Model(std::string path)
{
	m_path = path;
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

void Model::Draw(Matrix4 proj, Matrix4 view, Transform t)
{
	//m_models[objPath].m_shader->SetUniformMat4x4("projection", projection);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene));
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

MeshData Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
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
	material.shader = defaultShader;
	return MeshData(vertices, indices, material);
}

Material Model::ProcessMaterial(aiMaterial* material, std::string name)
{
	//load diffuse
	std::shared_ptr<TextureData> diffuse;
	aiString path;
	material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
	std::string spath = m_path + "/" + path.C_Str();
	diffuse = TextureLoader::Instance().LoadTexture(spath);

	//load specular
	std::shared_ptr<TextureData> specular;
	material->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &path);
	spath = m_path + "/" + path.C_Str();
	specular = TextureLoader::Instance().LoadTexture(spath);

	//load specular
	std::shared_ptr<TextureData> normalMaps;
	material->GetTexture(aiTextureType::aiTextureType_HEIGHT, 0, &path);
	spath = m_path + "/" + path.C_Str();
	normalMaps = TextureLoader::Instance().LoadTexture(spath);

	Material out;
	out.diffuse = diffuse;
	out.specular = specular;
	//out.normalmap = ...
	out.name = material->GetName().C_Str();
	out.shader = m_shader;

	

	return out;
}
