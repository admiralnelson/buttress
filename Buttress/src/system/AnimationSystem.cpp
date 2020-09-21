#include "pch.h"
#include "system/AnimationSystem.h"

void AnimationSystem::Init(Universe* universe)
{
	m_universe = universe;
}

bool AnimationSystem::CalculateBoneTransform(float atTime, EntityId e, std::vector<Transform>& results)
{
	Matrix4 identity = Matrix4();
	aiScene* scene;
	return true;
}

void AnimationSystem::RegisterAnimatedModel(std::string modelPath, const aiNode* modelRootNode)
{
}

void AnimationSystem::ReadNodeHierarchy(float atTime, const aiNode* node, Matrix4 parentTransform)
{
}

Vec3 AnimationSystem::CalcInterpolatedScaling(float atTime, const aiNodeAnim* nodeAnim)
{
	return Vec3();
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
	return 0;
}
