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
class OrbitCamera;
class Level {
public:
  Level();
  ~Level();
  void loadResources();
  void update(float dSecond);
  void render() const;
  void processInput(float dSecond);
  RenderScene& renderScene() { return *mRenderScene; }
  static Level& currentLevel();
protected:
  void updateLight();

  OrbitCamera* mCamera = nullptr;
  Camera* mDebugCamera = nullptr;
  Clock* mGameClock = nullptr;
  RenderScene* mRenderScene = nullptr;
  Player* mPlayer = nullptr;
  Light mSun;
  Map mMap;
};


