#include "pch.h"
#include "core/ModelData.h"
#include "components/Animation.h"
#include "Util.h"

std::shared_ptr<Shader> ModelData::defaultShader;
std::shared_ptr<Shader> ModelData::defaultAnimatedShader;
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
	if (scene->mMeshes[0]->mNumBones)
	{
		Animation animation;
		animation.numberOfBones = 0;
		animation.speed = 0;
		Matrix4 modelRootTransform = aiMatrix4x4ToMatrix4(scene->mRootNode->mTransformation);
		animation.modelInverseTransform = Inverse(modelRootTransform);
		e.AddComponent<Animation>(animation);
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
		m_meshes.push_back(ProcessMesh(mesh, scene, e));
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, e);
	}
}

MeshId ModelData::ProcessMesh(aiMesh* mesh, const aiScene* scene, Entity &e)
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
	if (mesh->mNumBones)
	{
		bones.resize(mesh->mNumVertices);
		Entity child = e.CreateEntity(RandomString(5));
		e.AttachChild(child);
		Animation& anim = e.GetComponent<Animation>();
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
				unsigned int vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
				bones[vertexId].AddBoneData(boneIdx, weight);
			}
		}
	}
	

	//materials
	aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
	MaterialId matId = MaterialLoader::Instance().LoadMaterial(ProcessMaterial(aimaterial, aimaterial->GetName().C_Str()));
	MaterialData& material = MaterialLoader::Instance().GetMaterialById(matId);
	if (mesh->mNumBones)
	{
		material.shader = defaultAnimatedShader; 
	}
	else
	{
		material.shader = defaultShader;
	}
	
	return MeshLoader::Instance().LoadMesh(mesh->mName.data, vertices, indices, bones, matId);
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

void ModelData::AttachAnimationToEntity(Entity& e)
{
	const aiScene* scene = m_importer->GetScene();
	if (scene->mMeshes[0]->mNumBones)
	{
		if (!e.IsComponentExist<Animation>())
		{
			Animation animation;
			animation.numberOfBones = 0;
			animation.speed = 0;
			Matrix4 modelRootTransform = aiMatrix4x4ToMatrix4(scene->mRootNode->mTransformation);
			animation.modelInverseTransform = Inverse(modelRootTransform);
			e.AddComponent<Animation>(animation);
		}
		AttachAnimationRecursively(scene->mRootNode, scene, e);
	}
	else
	{
		return;
	}
}

void ModelData::AttachAnimationRecursively(aiNode* node, const aiScene* scene, Entity& e)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		PopulateBoneData(mesh, scene, e);
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		AttachAnimationRecursively(node->mChildren[i], scene, e);
	}
}

void ModelData::PopulateBoneData(aiMesh* mesh, const aiScene* scene, Entity& e)
{
	if (mesh->mNumBones)
	{
		Entity child = e.CreateEntity(RandomString(5));
		e.AttachChild(child);
		Animation& anim = e.GetComponent<Animation>();
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

		}
	}
}

ModelLoader& ModelLoader::Instance()
{
	static ModelLoader loader;
	return loader;
}

bool ModelLoader::IsModelLoaded(ModelId id)
{
	return (id < m_modelCaches.size());
}

ModelId ModelLoader::LoadModel(std::string path, Entity& e)
{
	if (std::find(m_modelNames.begin(), m_modelNames.end(), path) == m_modelNames.end())
	{
		ModelData data(path, e);
		m_modelNames.push_back(path);
		m_modelCaches.push_back(data);
		return (ModelId)m_modelCaches.size() - 1;
	}
	else
	{
		auto it = std::find(m_modelNames.begin(), m_modelNames.end(), path);
		ModelId id = (ModelId)std::distance(m_modelNames.begin(), it);
		ModelData& model = GetModel(id);
		model.AttachAnimationToEntity(e);

		return id;
	}
}

ModelData& ModelLoader::GetModel(ModelId id)
{
	if (id >= m_modelCaches.size())
	{
		PRINT("ERROR", "modelId out of range! id", id);
		throw std::exception("model out of range");
	}
	return m_modelCaches[id];
}

bool ModelLoader::RegisterAnimation(Entity& e)
{
	return false;
}
