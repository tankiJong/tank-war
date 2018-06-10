#pragma once
#include "Engine/Core/common.hpp"
#include "Engine/Renderer/Camera.hpp"

class OrbitCamera: public Camera {
public:
  void rotate(float x, float y);
  void update();
  void processInput(float dSec);
  void attach(const Transform& t, float dist);
protected:
  float x = 0, y = 0;
  float distance;
  const Transform* target = nullptr;
};
