#include "pch.h"
#include "system/AnimationSystem.h"
#include "system/RenderSystem.h"
#include "components/Model.h"
#include "components/Animation.h"
#include "Util.h"

void AnimationSystem::Init(Universe* universe)
{
	m_universe = universe;
	m_startTime = GetCurrentTime();
}

void AnimationSystem::Tick(float dt)
{
	auto t1 = GetCurrentTime();
	std::lock_guard<std::mutex> secureThisBlock(m_mutex);
	float runningTime = (float)((double)GetCurrentTime() - (double)m_startTime) / 1000.0f;
	for (auto& e : m_entity)
	{
		Entity theEnt = m_universe->QueryByEntityId(e);
		CalculateBoneTransform(theEnt, runningTime);
	}
	PRINT("animation time (ms)", GetCurrentTime() - t1);
}

bool AnimationSystem::CalculateBoneTransform(Entity ent, float atTimeInSeconds)
{
	Matrix4 identity = Matrix4(1);
	RenderSystem* render = m_universe->GetSystem<RenderSystem>();
	Model& model = ent.GetComponent<Model>();
	if (render->m_models.size() <= model.id)
	{
		return false;
	}
	ModelData& modelData = render->m_models[model.id];
	Animation& anim = ent.GetComponent<Animation>();
	//not ready yet
	if (modelData.m_importer == nullptr)
	{
		return false;
	}
	const aiScene* scene = modelData.m_importer->GetScene();
	float tickPerSecond = 25;
	if (scene->mAnimations[0]->mTicksPerSecond)
	{
		tickPerSecond = (float)scene->mAnimations[0]->mTicksPerSecond;
	}
	float timeInTicks = atTimeInSeconds * tickPerSecond;
	float animationTime = fmod(timeInTicks, (float)scene->mAnimations[0]->mDuration);
	ReadNodeHierarchy(ent, scene, animationTime, scene->mRootNode, identity);
	anim.calculatedBonesMatrix.resize(anim.numberOfBones);
	for (size_t i = 0; i < anim.numberOfBones; i++)
	{
		anim.calculatedBonesMatrix[i] = anim.boneInfo[i].finalTransformation;
	}
	return true;
}


void AnimationSystem::ReadNodeHierarchy(Entity ent, const aiScene* model, float atTime, const aiNode* node, Matrix4 parentTransform)
{
	std::string nodeName(node->mName.data);
	const aiAnimation* animation = model->mAnimations[0];
	Matrix4 nodeTransform = aiMatrix4x4ToMatrix4(node->mTransformation);
	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);
	if (nodeAnim)
	{
		Vec3 scalling = CalcInterpolatedScaling(atTime, nodeAnim);
		Matrix4 scalMat = ScaleToMatrix4(scalling);

		Quaternion rotation;
		rotation = CalcInterpolatedRotation(atTime, nodeAnim);
		Matrix4 rotMat = QuaternionToMatrix4(rotation);

		Vec3 translation = CalcInterpolatedPosition(atTime, nodeAnim);
		Matrix4 transMat = TranslationToMatrix4(translation);

		nodeTransform = transMat * rotMat * scalMat;

	}

	Matrix4 globalTransform = parentTransform * nodeTransform;

	Animation& anim = ent.GetComponent<Animation>();
	if (anim.boneNameToIndex.find(nodeName) != anim.boneNameToIndex.end())
	{
		unsigned int boneIndex = anim.boneNameToIndex[nodeName];
		Matrix4& rootMat = anim.modelInverseTransform;
		anim.boneInfo[boneIndex].finalTransformation = rootMat * globalTransform * anim.boneInfo[boneIndex].boneOffset;
		//TODO: REPOSITION THE CHILD ENTITIES WHICH ATTACHED TO THE MODEL!
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ReadNodeHierarchy(ent, model, atTime, node->mChildren[i], globalTransform);
	}
}

Vec3 AnimationSystem::CalcInterpolatedScaling(float atTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1)
	{
		aiVector3D& scallingKey = nodeAnim->mScalingKeys[0].mValue;
		return { scallingKey.x, scallingKey.y, scallingKey.z };
	}

	unsigned int scallingIndex = FindScaling(atTime, nodeAnim);
	unsigned int nextScallingIndex = scallingIndex + 1;
	if (nextScallingIndex > nodeAnim->mNumScalingKeys)
	{
		PRINT("ERROR", "next scalling index out of range, aiNodeAnim ptr", nodeAnim);
		throw std::exception("next scalling index out of range");
	}
	float deltaTime = (float)(nodeAnim->mScalingKeys[nextScallingIndex].mTime - nodeAnim->mScalingKeys[scallingIndex].mTime);
	float factor = (atTime - (float)nodeAnim->mScalingKeys[scallingIndex].mTime) / deltaTime;
	if (factor < 0 || factor > 1)
	{
		PRINT("ERROR", "factor is out of range [0..1] aiNodeAnim ptr", nodeAnim, "factor value", factor);
		throw std::exception("factor is out of range");
	}
	aiVector3D& start = nodeAnim->mScalingKeys[scallingIndex].mValue;
	aiVector3D& end = nodeAnim->mScalingKeys[nextScallingIndex].mValue;
	aiVector3D deltaV = end - start;
	return { deltaV.x, deltaV.y, deltaV.z };
}

Vec3 AnimationSystem::CalcInterpolatedPosition(float atTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumPositionKeys == 1)
	{
		aiVector3D& positionKey = nodeAnim->mPositionKeys[0].mValue;
		return { positionKey.x, positionKey.y, positionKey.z };
	}

	unsigned int positionIndex = FindPosition(atTime, nodeAnim);
	unsigned int nextPositionIndex = positionIndex + 1;
	if (nextPositionIndex > nodeAnim->mNumPositionKeys)
	{
		PRINT("ERROR", "next scalling index out of range, aiNodeAnim ptr", nodeAnim);
		throw std::exception("next scalling index out of range");
	}
	float deltaTime = (float)(nodeAnim->mPositionKeys[nextPositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
	float factor = (atTime - (float)nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
	if (factor < 0 || factor > 1)
	{
		PRINT("ERROR", "factor is out of range [0..1] aiNodeAnim ptr", nodeAnim, "factor value", factor);
		throw std::exception("factor is out of range");
	}
	aiVector3D& start = nodeAnim->mPositionKeys[positionIndex].mValue;
	aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIndex].mValue;
	aiVector3D deltaV = end - start;
	aiVector3D out = start + factor * deltaV;
	return { out.x, out.y, out.z };
}

Quaternion AnimationSystem::CalcInterpolatedRotation(float atTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumRotationKeys == 1)
	{
		return aiQuaternionToQuaternion(nodeAnim->mRotationKeys[0].mValue);
	}

	unsigned int rotationIndex = FindRotation(atTime, nodeAnim);
	unsigned int nextRotationIndex = rotationIndex + 1;
	if (nextRotationIndex > nodeAnim->mNumRotationKeys)
	{
		PRINT("ERROR", "next rotation index out of range, aiNodeAnim ptr", nodeAnim);
		throw std::exception("next scalling index out of range");
	}
	float deltaTime = (float)(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
	float factor = (atTime - (float)nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
	if (factor < 0 || factor > 1)
	{
		PRINT("ERROR", "factor is out of range [0..1] aiNodeAnim ptr", nodeAnim, "factor value", factor);
		throw std::exception("factor is out of range");
	}
	const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[rotationIndex].mValue;
	const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[nextRotationIndex].mValue;
	aiQuaternion out = aiQuaternion();
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, factor);
	out = out.Normalize();
	return aiQuaternionToQuaternion(out);
}

const aiNodeAnim* AnimationSystem::FindNodeAnim(const aiAnimation* anim, std::string nodeName)
{
	for (size_t i = 0; i < anim->mNumChannels; i++)
	{
		const aiNodeAnim* nodeAnim = anim->mChannels[i];
		if (std::string(nodeAnim->mNodeName.data) == nodeName)
		{
			return nodeAnim;
		}
	}
	return nullptr;
}

unsigned int AnimationSystem::FindScaling(float atTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumScalingKeys <= 0)
	{
		PRINT("ERROR", "unable to find scalling index for nodeAnim:", nodeAnim->mNodeName.data, "at ptr", nodeAnim, "it is empty");
		throw std::exception("there is no scaling keys!");
		return -1;
	}

	for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
	{
		if (atTime <= (float)nodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	PRINT("ERROR", "unable to find scalling index for nodeAnim:", nodeAnim->mNodeName.data, "at ptr", nodeAnim, "it is cannot be found");
	throw std::exception("cannot find the scalling position");
	return -1;
}

unsigned int AnimationSystem::FindRotation(float atTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumRotationKeys == 0)
	{
		PRINT("ERROR", "unable to find rotation index for nodeAnim:", nodeAnim->mNodeName.data, "at ptr", nodeAnim, "it is empty");
		throw std::exception("there is no rotation keys!");
		return -1;
	}

	for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
	{
		if (atTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	PRINT("ERROR", "unable to find rotation index for nodeAnim:", nodeAnim->mNodeName.data, "at ptr", nodeAnim, "it is cannot be found");
	throw std::exception("cannot find the rotation index");
	return -1;
}

unsigned int AnimationSystem::FindPosition(float atTime, const aiNodeAnim* nodeAnim)
{
	for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
	{
		if (atTime <= (float)nodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	PRINT("ERROR", "unable to find position index for nodeAnim:", nodeAnim->mNodeName.data, "at ptr", nodeAnim, "it is cannot be found");
	throw std::exception("cannot find the position index");
	return -1;
}
