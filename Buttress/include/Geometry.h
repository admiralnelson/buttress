#pragma once
#define GLM_ENABLE_EXPERIMENTAL 1
#include "glm/common.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"
#include "assimp/matrix4x4.h"
#include "assimp/matrix3x3.h"
#include "assimp/quaternion.h"

typedef glm::tvec2<float> Vec2;
typedef glm::tvec3<float> Vec3;
typedef glm::tvec4<float> Vec4;

typedef glm::tvec2<int> IntVec2;

typedef glm::quat Quaternion;
typedef glm::mat4x4 Matrix4;

struct Vertex
{
	Vec3 position = { 0,0,0 };
	Vec3 normal = { 0,0,0 };
	Vec2 uv = { 0,0 };
	Vec2 uv2 = { 0,0 };
};

Matrix4 Rotate(Matrix4 &input, Vec3 axis);
Vec3 Quaternion2RotationEuler(Quaternion &input);
Vec3 Quaternion2RotationEulerDeg(Quaternion &input);

Quaternion aiQuaternionToQuaternion(const aiQuaternion& q);
Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& m);
Matrix4 aiMatrix3x3ToMatrix4(const aiMatrix3x3& m);
Matrix4 QuaternionToMatrix4(const Quaternion& input);
Matrix4 ScaleToMatrix4(Vec3& v);
Matrix4 TranslationToMatrix4(Vec3& v);
void DecomposeMatrix4(Matrix4& mat, Vec3& outScale, Quaternion& outRot, Vec3& outTrans, Vec3& outSkew, Vec4& outPers);
Matrix4 Inverse(Matrix4& in);