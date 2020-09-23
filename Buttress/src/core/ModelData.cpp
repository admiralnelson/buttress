#include "pch.h"
#include "core/ModelData.h"
#include "components/Animation.h"
#include "Util.h"

std::shared_ptr<Shader> ModelData::defaultShader;
ModelData::ModelData(std::string path, Entity& e)
{
	m_path = path;
	m_importer.reset(new Assimp::Importer());
	const aiScene* scene = m_importer->ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals  | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		PRINT("ERROR", "unable to load file", path);
		PRINT("     ", "reason:", m_importer->GetErrorString());
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
	ProcessNode(scene->mRootNode, scene, e);
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

void ModelData::ProcessNode(aiNode* node, const aiScene* scene, Entity& e)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(MeshLoader::LoadMesh(ProcessMesh(mesh, scene, e)));
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, e);
	}
}

MeshData ModelData::ProcessMesh(aiMesh* mesh, const aiScene* scene, Entity &e)
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
	if (0)
	{
		Entity child = e.CreateEntity(RandomString(5));
		e.AttachChild(child);
		Animation& anim = e.GetComponent<Animation>(); //TODO: we need THE ROOT entity to store the animation!
		anim.numberOfBones = 0;
		for (size_t i = 0; i < mesh->mNumBones; i++)
		{
			unsigned int boneIdx = 0;
			std::string boneName = mesh->mBones[i]->mName.C_Str();
			if (anim.boneNameToIndex.find(boneName) == anim.boneNameToIndex.end())
			{
				boneIdx = anim.numberOfBones;
				anim.numberOfBones++;
				Animation::BoneInfo info;
				anim.boneInfo.push_back(info);
				anim.boneInfo[boneIdx].boneOffset = aiMatrix4x4ToMatrix4(mesh->mBones[i]->mOffsetMatrix);
				anim.boneNameToIndex[boneName] = boneIdx;
				Transform& refChildTransform = child.GetComponent<Transform>();
				child.GetComponent<EntityName>().name += boneName;
				Vec3 dummy;
				Vec4 dummy2;
				Matrix4 rootMatrix = e.GetComponent<Transform>().GetTransform();
				Matrix4 relativeToRoot = rootMatrix * anim.boneInfo[boneIdx].boneOffset;
				DecomposeMatrix4(relativeToRoot, refChildTransform.scale, refChildTransform.localRotation, refChildTransform.position, dummy, dummy2);
			}
			else
			{
				boneIdx = anim.boneNameToIndex[boneName];
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
	}
	

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
	std::shared_ptr<TextureData> diffuse = nullptr;
	aiString path;
	material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
	if (strlen(path.C_Str()) != 0)
	{
		std::string spath = m_path + "/" + path.C_Str();
		diffuse = TextureLoader::Instance().LoadTexture(spath);
	}

	//load specular
	std::shared_ptr<TextureData> specular = nullptr;
	material->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &path);
	if (strlen(path.C_Str()) != 0)
	{
		std::string spath = m_path + "/" + path.C_Str();
		specular = TextureLoader::Instance().LoadTexture(spath);
	}

	//load specular
	std::shared_ptr<TextureData> normalMaps = nullptr;
	material->GetTexture(aiTextureType::aiTextureType_HEIGHT, 0, &path);
	if (strlen(path.C_Str()) != 0)
	{
		std::string spath = m_path + "/" + path.C_Str();
		normalMaps = TextureLoader::Instance().LoadTexture(spath);
	}

	MaterialData out(name, m_shader);
	out.diffuse = diffuse;
	out.specular = specular;
	//out.normalmap = ...

	return out;
}
