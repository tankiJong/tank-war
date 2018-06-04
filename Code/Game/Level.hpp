#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Effect/ParticleEmitter.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/Player.hpp"
#include "Engine/Framework/Light.hpp"
class SpriteSheet;
class Clock;


class Mesh;

class Level {
public:
  Level();
  ~Level();
  void loadResources();
  void update(float dSecond);
  void render() const;
  void processInput(float dSecond);


protected:
  void updateLight();

  Camera* mCamera;
  Clock* mGameClock;
  RenderScene* mRenderScene;
  Light mSun;
  Map mMap;
  Player mPlayer;
};


