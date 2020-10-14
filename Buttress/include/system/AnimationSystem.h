#pragma once
#include "pch.h"
#include "ecs/ECS.h"
#include "assimp/scene.h"
#include "components/Transform.h"
#include "components/Animation.h"
#include "system/RenderSystem.h"

class AnimationSystem : public System
{
	friend class RenderSystem;
public:
	void Init(Universe* universe) override;
	void Start();
	void Process(size_t index);
	void Tick(float deltaT);
	bool CalculateBoneTransform(Entity ent, float atTimeInSeconds);
	void PushAnimationData(EntityId ent, Animation animationData);
	size_t GetTotalEntity();

private:
	void ReadNodeHierarchy(Entity ent, const aiScene* model, float atTime, const aiNode* node, const Matrix4& parentTransform, Animation& animation);

	Vec3 CalcInterpolatedScaling(float atTime, const aiNodeAnim* nodeAnim);
	Vec3 CalcInterpolatedPosition(float atTime, const aiNodeAnim* nodeAnim);
	Quaternion CalcInterpolatedRotation(float atTime, const aiNodeAnim* nodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* anim, const char* nodeName);
	unsigned int FindScaling(float atTime, const aiNodeAnim* nodeAnim);
	unsigned int FindRotation(float atTime, const aiNodeAnim* nodeAnim);
	unsigned int FindPosition(float atTime, const aiNodeAnim* nodeAnim);
	std::unordered_map<std::string, aiNode*> m_modelWithAnimationCache;
	std::unordered_map<EntityId, std::vector<Transform>> m_entityToBonesTransforms;
	long long m_startTime = 0;

	std::deque<std::pair<EntityId, Animation>> m_entitiesToProcess;

	RenderSystem* renderer = nullptr;
};