#pragma once
#include "Engine/Core/common.hpp"
#include "Game/Gameplay/Entity.hpp"
#include "Game/Gameplay/Turret.hpp"

class Tank : public Entity {
public:
  uint teamId;
  Turret turret;
  void init() override;
  void turn(float degSec);
  void turnToward(const vec3& target, float speedDeg);
  void advance(float speedSec);
  void advance(float speedSec, const vec3 norm);
  void turnTurret(float degSec);
  void shoot();
};
