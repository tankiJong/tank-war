#pragma once
#include "Engine/Math/Primitives/vec3.hpp"

class mat44;
class Transform;

void turnToward(Transform& current, const vec3& targetInWorld, float angleDegMax);