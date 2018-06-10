#pragma once
#include "Engine/Core/common.hpp"
#include "Engine/Renderer/Renderable/Renderable.hpp"
#include "Engine/Math/Transform.hpp"

class Entity {
public:
  Transform transform;
  Renderable renderable;
  Entity();
  virtual ~Entity();
  virtual void init() {};
  virtual void update(float dSecond) {};
  virtual void onInput(float dSecond) {};
};
