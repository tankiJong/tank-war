#pragma once
#include "Game/Gameplay/Turret.hpp"
#include "Game/Gameplay/Tank.hpp"

class Player: public Tank {
public:
  static constexpr int MAX_HEALTH = 10000;
  void update(float dSecond) override;
  void onInput(float dSecond) override;
  inline void cutHealth(int h) { currentHealth -= h; }
  inline bool isDead() const override { return currentHealth < 0; }
  inline float healthScaled() const { return (float)currentHealth / (float)MAX_HEALTH; }
  inline void recovery() { currentHealth = MAX_HEALTH; }
  ~Player();

protected:
  int currentHealth = MAX_HEALTH;
};
