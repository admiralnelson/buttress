#pragma once
#include "pch.h"
#include "ecs/ECS.h"
#include "components/Transform.h"
#include "assimp/scene.h"

class AnimationSystem : public System
{
	friend class RenderSystem;
public:
	void Init(Universe* universe);
	bool CalculateBoneTransform(float atTime, EntityId e, std::vector<Transform>& results);
private:
	void RegisterAnimatedModel(std::string modelPath, const aiNode* modelRootNode);
	void ReadNodeHierarchy(float atTime, const aiNode* node, Matrix4 parentTransform);

	Vec3 CalcInterpolatedScaling(float atTime, const aiNodeAnim* nodeAnim);
	Vec3 CalcInterpolatedPosition(float atTime, const aiNodeAnim* nodeAnim);
	Quaternion CalcInterpolatedRotation(float atTime, const aiNodeAnim* nodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* anim, std::string nodeName);
	unsigned int FindScaling(float atTime, const aiNodeAnim* nodeAnim);
	unsigned int FindRotation(float atTime, const aiNodeAnim* nodeAnim);
	unsigned int FindPosition(float atTime, const aiNodeAnim* nodeAnim);
	std::unordered_map<std::string, aiNode*> m_modelWithAnimationCache;
	std::unordered_map<EntityId, std::vector<Transform>> m_entityToBonesTransforms;
};