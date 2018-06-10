#pragma once
#include "Game/Gameplay/Turret.hpp"
#include "Game/Gameplay/Tank.hpp"

class Player: public Tank {
public:
  void update(float dSecond) override;
  void onInput(float dSecond) override;

  ~Player();
};
