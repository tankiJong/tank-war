#pragma once
#include "Engine/Core/common.hpp"
#include "Game/Gameplay/Entity.hpp"
#include "Engine/Math/Primitives/sphere.h"

class Enemy: public Entity {
public:
  static constexpr float ENEMY_SIZE = .2f;
  void init() override;
  Entity* target = nullptr;
  vec3 velocity = vec3::zero;
  sphere bound() const { return { transform.position(), ENEMY_SIZE }; }
  virtual void update(float dSecond) override;
};
