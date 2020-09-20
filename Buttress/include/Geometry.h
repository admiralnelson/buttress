#pragma once
#define GLM_ENABLE_EXPERIMENTAL 1
#include "glm/common.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"
typedef glm::tvec2<float> Vec2;
typedef glm::tvec3<float> Vec3;

typedef glm::quat Quaternion;
typedef glm::mat4x4 Matrix4;

Matrix4 Rotate(Matrix4 &input, Vec3 axis);
Vec3 Quaternion2RotationEuler(Quaternion &input);
Vec3 Quaternion2RotationEulerDeg(Quaternion &input);
