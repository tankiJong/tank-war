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
  Renderable text;
  Renderable background;

  void init(std::string_view txt, const Rgba& color);

  void regScene(RenderScene& scene) {
    scene.add(background);
    scene.add(text);
  }

  void unregScene(RenderScene& scene) {
    scene.remove(text);
    scene.remove(background);
  }

protected:
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
  RenderScene* mRenderScene = nullptr;
  SceenWord mUis[NUM_STATES];
};
