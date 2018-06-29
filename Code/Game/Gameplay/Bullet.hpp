#pragma once
#include "Engine/Core/common.hpp"
#include "Game/Gameplay/Entity.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Math/Primitives/sphere.h"

class Bullet: public Entity {
public:
  static constexpr float MAX_AGE = 5.f;
  static constexpr float BULLET_SIZE = .05f;
  Bullet() = default;

  void init() override;

  void update(float dSecond) override;
  //
  // void onInput(float dSecond) override;
  //

  vec3 velocity;

  sphere bound() const;

  ~Bullet();
protected:
  Interval life;
};
