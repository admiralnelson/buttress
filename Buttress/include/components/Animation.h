#pragma once
#include "pch.h"

struct Animation
{
	std::string currentAnimation;
	float speed;
private:
	friend class ModelData;
	friend class AnimationSystem;
	friend class RenderSystem;
	struct BoneInfo
	{
		Matrix4 boneOffset = Matrix4();
		Matrix4 finalTransformation = Matrix4();
	};
	Matrix4 modelInverseTransform;
	std::unordered_map<std::string, unsigned int> boneNameToIndex;
	std::vector<BoneInfo> boneInfo;
	std::vector<Matrix4> calculatedBonesMatrix;
	unsigned int numberOfBones;
};