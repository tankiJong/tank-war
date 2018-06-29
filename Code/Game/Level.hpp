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
class Bullet;
class Enemy;
class EnemySpawner;
class Level;

class HUD {
public:
  Level& level;
  HUD(Level& level) : level(level) {}

  void render() const;
};


class Level {
public:
  friend class HUD;
  Level();
  ~Level();
  void loadResources();
  void update(float dSecond);
  void render() const;
  void processInput(float dSecond);
  RenderScene& renderScene() { return *mRenderScene; }
  const Map& map() { return mMap; }
  void swam(Enemy& e);
  template<typename EntityType> EntityType* addEntity(const vec3& position, const Transform* parent = nullptr);
  template<typename EntityType> void removeEntity(EntityType& e);

  bool isPlayerWin();

  static Level& currentLevel();
  void afterFrame();

  Player* mPlayer = nullptr;
  std::vector<Enemy*> mEnemies;
protected:
  void updateLight();
  std::vector<Entity*> mEntities;
  std::vector<Bullet*> mBullets;
  std::vector<Tank*> mTanks;
  std::vector<EnemySpawner*> mEnemySpawners;

  HUD mHud;
  OrbitCamera* mCamera = nullptr;
  Camera* mDebugCamera = nullptr;
  Clock* mGameClock = nullptr;
  RenderScene* mRenderScene = nullptr;
  Light mSun;
  Map mMap;
};


