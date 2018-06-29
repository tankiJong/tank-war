#include "OrbitCamera.hpp"
#include "Engine/Renderer/Shader/ShaderPass.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Debug/Draw.hpp"

void OrbitCamera::update() {
  vec3 targetWorld = target->position() + vec3::up;
  // Debug::drawCube(targetWorld, 1.f, true, 0);
  vec4 camWorld = vec4(fromSpherical(distance, y, x) + targetWorld, 1.f);
  Debug::drawCube(targetWorld, .1f, true, 0);
  Debug::drawCube(camWorld.xyz(), .3f, true, 0, Rgba::black);
  //
  // mat44 worldView = mat44::lookAt(camWorld.xyz(), targetWorld);
  //
  // mat44 camToTarget = transfrom().parent()->localToWorld().inverse() * worldView;
  // transfrom().setlocalTransform(camToTarget.inverse());
  lookAt(camWorld.xyz(), targetWorld);
  // Debug::drawBasis(transfrom().position(), transfrom().right(), transfrom().up(), transfrom().forward(), 10.f);
}

void OrbitCamera::processInput(float dSec) {
  vec2 mouseDelta = g_theInput->mouseDeltaPosition(false);

  vec3 d(mouseDelta.y, mouseDelta.x, 0);

  if (d.magnitude() > 0) {
    //    d = d.normalized();
    d.x *= dSec;
    d.y *= dSec;
  }

  x -= d.x;
  y -= d.y;

  x = std::clamp(x, 0.f, 15.f);
}

void OrbitCamera::attach(const Transform& t, float dist) {
  target = &t;
  // transfrom().parent() = &t;
  x = 0; y = 0;
  distance = dist;
}
