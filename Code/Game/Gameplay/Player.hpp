#pragma once
#include "Engine/Core/common.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable/Renderable.hpp"

class Player {
public:
  Transform transform;
  Renderable renderable;
  void init();
  ~Player();
};
