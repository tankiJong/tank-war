#pragma once
#include "Engine/Core/common.hpp"
#include "Game/Gameplay/Entity.hpp"
#include "Game/Gameplay/Turret.hpp"

class Tank : public Entity {
public:
  Turret turret;
  void init() override;
  virtual ~Tank();
};
