#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Effect/ParticleEmitter.hpp"
class SpriteSheet;
class Clock;


class Mesh;

class Game {
public:
  Game();
  ~Game();
  void loadResources();
  void update(float dSecond);
  void render() const;
  void processInput(float dSecond);
  void beforeFrame();
  void afterFrame();

protected:

  void updateLight();
  Camera * mCamera;
  Clock* mGameClock;
  RenderScene* mRenderScene;
};

extern Game* g_theGame;

