#pragma once
#include "Engine/Core/common.hpp"
#include "Game/Gameplay/Entity.hpp"
#include "Engine/Math/Primitives/aabb3.hpp"
#include "Engine/Core/Time/Clock.hpp"

class Enemy;
class EnemySpawner: public Entity {
public:
  void init() override;

  void update(float dSecond) override;

  aabb3 bound() const;
  owner<Enemy*> spawn() const;

  Interval interval;

protected:
  aabb3 mBound;
};
