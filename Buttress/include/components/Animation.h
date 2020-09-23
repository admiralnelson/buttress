#pragma once
#include "pch.h"

struct Animation
{
	std::string currentAnimation;
	float speed;
private:
	friend class ModelData;
	friend class AnimationSystem;
	struct BoneInfo
	{
		Matrix4 boneOffset = Matrix4();
		Matrix4 finalTransformation = Matrix4();
	};

	std::unordered_map<std::string, unsigned int> boneNameToIndex;
	std::vector<BoneInfo> boneInfo;
	unsigned int numberOfBones;
};