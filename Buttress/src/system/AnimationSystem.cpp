#include "pch.h"
#include "system/AnimationSystem.h"
#include "system/RenderSystem.h"
#include "components/Model.h"

void AnimationSystem::Init(Universe* universe)
{
	m_universe = universe;
}

bool AnimationSystem::CalculateBoneTransform(float atTime, EntityId e, std::vector<Matrix4>& results)
{
	Matrix4 identity = Matrix4();
	Entity ent = m_universe->QueryByEntityId(e);
	RenderSystem* render = m_universe->GetSystem<RenderSystem>();
	Model& model = ent.GetComponent<Model>();
	ModelData& modelData = render->m_models[model.id];

	const aiScene* scene = modelData.m_importer->GetScene();
	float tickPerSecond = 25;
	if (scene->mAnimations[0]->mTicksPerSecond)
	{
		tickPerSecond = (float)scene->mAnimations[0]->mTicksPerSecond;
	}
	float timeInTicks = atTime * tickPerSecond;
	float animationTime = fmod(timeInTicks, (float)scene->mAnimations[0]->mDuration);
	ReadNodeHierarchy(scene, animationTime, scene->mRootNode, identity);
	results.resize(modelData.m_numberOfBones);
	for (size_t i = 0; i < modelData.m_numberOfBones; i++)
	{
		results[i] = modelData.m_boneInfos[i].finalTransformation;
	}
	return true;
}

void AnimationSystem::RegisterAnimatedModel(std::string modelPath, const aiNode* modelRootNode)
{
}

void AnimationSystem::ReadNodeHierarchy(const aiScene* model, float atTime, const aiNode* node, Matrix4 parentTransform)
{
	std::string nodeName(node->mName.data);
	const aiAnimation* animation = model->mAnimations[0];
	Matrix4 nodeTransform = aiMatrix4x4ToMatrix4(node->mTransformation);
	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);
	if (nodeAnim)
	{
		Vec3 scalling = CalcInterpolatedScaling(atTime, nodeAnim);
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
	float deltaTime = (float)(nodeAnim->mScalingKeys[scallingIndex].mTime - nodeAnim->mScalingKeys[nextScallingIndex].mTime);
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
	return Vec3();
}

Quaternion AnimationSystem::CalcInterpolatedRotation(float atTime, const aiNodeAnim* nodeAnim)
{
	return Quaternion();
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
	return 0;
}

unsigned int AnimationSystem::FindRotation(float atTime, const aiNodeAnim* nodeAnim)
{
	return 0;
}

unsigned int AnimationSystem::FindPosition(float atTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 0)
	{
		PRINT("ERROR", "unable to find scalling position for nodeAnim:", nodeAnim->mNodeName.data, "at ptr", nodeAnim, "it is empty");
		throw std::exception("there is no scaling keys!");
	}

	for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
	{
		if (atTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	PRINT("ERROR", "unable to find scalling position for nodeAnim:", nodeAnim->mNodeName.data, "at ptr", nodeAnim, "it is cannot be found");
	throw std::exception("cannot find the scalling position");
	return -1;
}
