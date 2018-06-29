#include "util.hpp"
#include "Engine/Math/Primitives/mat44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"

void turnToward(Transform& current, const vec3& target, float angleDegMax) {
  float f = current.localRotation().magnitude2();
  EXPECTS(f >= 0);
  if ((target - current.position()).magnitude2() < 0.001f) return;
  vec3 forward = (target - current.position()).normalized();
  EXPECTS(equal(forward.magnitude(), 1.f));
  vec3 currentForward = current.forward().normalized();
  float angle = forward.angle(currentForward);

  if(equal(angle, 0.f)) {
    return;
  }

  if(angle < 0) {
    angleDegMax = -angleDegMax;
  }

  float frac = clampf01(angleDegMax / angle);

  forward = slerp(currentForward, forward, frac).normalized();
  EXPECTS(equal(forward.magnitude(), 1.f));

  vec3 right = current.up().cross(forward).normalized();
  vec3 up = forward.cross(right);

  mat44 worldToCurrent = mat44(right, up, forward, current.position());
  ENSURES(current.localRotation().magnitude2() >= 0);

  current.setWorldTransform(worldToCurrent);
  ENSURES(current.localRotation().magnitude2() >= 0);

}

