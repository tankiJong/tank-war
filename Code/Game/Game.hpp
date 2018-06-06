#pragma once
#include "Engine/Core/common.hpp"
#include "Game/Level.hpp"

enum eGameState {
  STATE_LOADING,
  STATE_MAIN_MENU,
  STATE_READY_UP,
  STATE_LEVEL,
  NUM_STATES,
};


struct SceenWord {
public:
  void init(std::string_view txt, const Rgba& color);

  inline void render() const {
    func();
  }
protected:
  std::function<void()> func;
  Transform transform;
};


class Game {
public:
  Game();
  void beforeFrame();
  void afterFrame();
  void render() const;

  void loadResources() const;

  void update(float dSecond);
  void processInput(float dSecond);
  eGameState state = STATE_LOADING;
protected:
  void switchState(eGameState to);
  Level* mLevel = nullptr;
  SceenWord mUis[NUM_STATES];
};
