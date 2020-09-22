#include "pch.h"
#include "core/ModelData.h"

std::shared_ptr<Shader> ModelData::defaultShader;
ModelData::ModelData(std::string path)
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

void ModelData::Draw(Matrix4 proj, Matrix4 view, Matrix4 model)
{
	//TODO: -> instead of drawing, push it to the queue
	//m_models[objPath].m_shader->SetUniformMat4x4("projection", projection);
	//for (auto& i : m_meshes)
	//{
	////	i.Draw(proj, view, model);
	//}
}

void ModelData::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(MeshLoader::LoadMesh(ProcessMesh(mesh, scene)));
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

MeshData ModelData::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<VertexBoneData> bones;


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

	//TODO: PARSE the bones.
	//bones

	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int boneIdx = 0;
		std::string boneName = mesh->mBones[i]->mName.C_Str();
		if (m_boneNameToIndex.find(boneName) == m_boneNameToIndex.end())
		{
			boneIdx = m_numberOfBones;
			m_numberOfBones++;
			BoneInfo info;
			m_boneInfos.push_back(info);
			m_boneInfos[boneIdx].boneOffset = aiMatrix4x4ToMatrix4(mesh->mBones[i]->mOffsetMatrix);
			m_boneNameToIndex[boneName] = boneIdx;
		}
		else
		{
			boneIdx = m_boneNameToIndex[boneName];
		}

		VertexBoneData boneData{};
		boneData.ids.fill(0);
		boneData.weights.fill(0);
		for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{	
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			boneData.ids[j] = mesh->mBones[i]->mWeights[j].mVertexId; 
			boneData.weights[j] = mesh->mBones[i]->mWeights[j].mWeight;
			bones.push_back(boneData);
		}
	}

	//save animation data & bones to this object
	aiAnimation;

	//materials
	aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
	MaterialId matId = MaterialLoader::LoadMaterial(ProcessMaterial(aimaterial, aimaterial->GetName().C_Str()));
	MaterialData& material = MaterialLoader::GetMaterialById(matId);
	material.shader = defaultShader;
	
	return MeshData(vertices, indices, bones, matId);
}

MaterialData ModelData::ProcessMaterial(aiMaterial* material, std::string name)
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

	MaterialData out(name, m_shader);
	out.diffuse = diffuse;
	out.specular = specular;
	//out.normalmap = ...

	return out;
}
