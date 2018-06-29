#include "Engine/Math/Primitives/vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Level.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Debug/Console/Console.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Renderer/Font.hpp"
#include "Engine/Renderer/Shader/Shader.hpp"
#include "Engine/Renderer/ForwardRendering.hpp"
#include "Engine/Debug/Log.hpp"
#include "Engine/File/FileSystem.hpp"
#include "Game/Gameplay/OrbitCamera.hpp"
#include "Game/Gameplay/Bullet.hpp"
#include "Engine/Math/Primitives/ray3.hpp"
#include "Game/Gameplay/Enemy.hpp"
#include "Game/Gameplay/EnemySpawner.hpp"
Level* gCurrentLevel = nullptr;

template<typename T>
bool removeFromList(std::vector<T*>& list, T& ele) {
  for(size_t i = list.size() - 1; i < list.size(); --i) {
    if(&ele == list[i]) {
      std::swap(list[i], list.back());
      list.pop_back();
      return true;
    }
  }
  return false;
}

template<>
Bullet* Level::addEntity(const vec3& position, const Transform* parent) {
  Bullet* b = new Bullet();
  b->init();
  b->transform.localTranslate(position);
  b->transform.parent() = parent;
  mEntities.push_back(b);
  mBullets.push_back(b);
  return b;
}

template<>
Enemy* Level::addEntity(const vec3& position, const Transform* parent) {
  Enemy* e = new Enemy();
  e->init();
  e->target = mPlayer;
  e->transform.localTranslate(position);
  e->transform.parent() = parent;
  mEntities.push_back(e);
  mEnemies.push_back(e);
  return e;
}

template<> void Level::removeEntity(Bullet& b) {
  bool re1 = removeFromList(mBullets, b);
  bool re2 = removeFromList<Entity>(mEntities, b);

  delete &b;
  ENSURES(re1 && re2);
}

template<> void Level::removeEntity(EnemySpawner& b) {
  bool re1 = removeFromList(mEnemySpawners, b);
  bool re2 = removeFromList<Entity>(mEntities, b);

  delete &b;
  ENSURES(re1 && re2);
}

template<> void Level::removeEntity(Enemy& b) {
  bool re1 = removeFromList(mEnemies, b);
  bool re2 = removeFromList<Entity>(mEntities, b);

  delete &b;
  ENSURES(re1 && re2);
}

Level::Level(): mRenderScene(new RenderScene()), mHud(*this) {
  gCurrentLevel = this;
  mCamera = new OrbitCamera();
  mCamera->setProjectionPrespective(30.f, 5.f*CLIENT_ASPECT, 5.f, .1f, 100.f);
  mCamera->setColorTarget(g_theRenderer->getDefaultColorTarget());
  mCamera->setDepthStencilTarget(g_theRenderer->getDefaultDepthTarget());
  mCamera->setFlag(CAM_CLEAR_DEPTH | CAM_CLEAR_COLOR | CAM_EFFECT_BLOOM);

  mDebugCamera = new Camera();
  mDebugCamera->setProjectionPrespective(30.f, 5.f*CLIENT_ASPECT, 5.f, .1f, 100.f);
  mDebugCamera->setColorTarget(g_theRenderer->getDefaultColorTarget());
  mDebugCamera->setDepthStencilTarget(g_theRenderer->getDefaultDepthTarget());
  mDebugCamera->setFlag(CAM_CLEAR_DEPTH | CAM_CLEAR_COLOR | CAM_EFFECT_BLOOM);
  mDebugCamera->transfrom().setlocalTransform(mat44::lookAt({ -15, 10, 10 }, vec3::zero));
  Debug::setCamera(mCamera);

  mCamera->transfrom().localRotation() = vec3::zero;
  //mCamera->translate({ 3.f, 3.f, 3.f });
  //mCamera->rotate({ -45.f, 135.f, 0.f });

  g_theUiCamera->sort = 1;
  mCamera->sort = 0;

  loadResources();

  mPlayer = new Player();
  mPlayer->init();
  mPlayer->transform.localTranslate({ 10, 0, 10 });
  mCamera->attach(mPlayer->transform, 5.f);
  mMap.init();

  mSun.transform.setlocalTransform(mat44::lookAt(vec3{-1.f ,10.f, -1.f}, vec3(10, 0, 10)));
  // mSun.transform.parent() = &mPlayer->transform;
  Debug::drawCone(mSun.transform.position(), mSun.transform.forward(), 2.f, 10.f, 10);
  Debug::drawBasis(mSun.transform.position(), mSun.transform.right(), mSun.transform.up(), mSun.transform.forward());
  mSun.asDirectionalLight(3.f, vec3(1, 0, 0));
  mSun.castShadow = true;

  EnemySpawner* es = new EnemySpawner();
  es->init();
  es->transform.localTranslate({ 20.f, 0, 20.f });
  float height = mMap.height(es->transform.position().xz());
  EXPECTS(height <= 10.f && height >= -10.f);
  es->transform.localPosition().y = height;
  mEnemySpawners.push_back(es);
  mEntities.push_back(es);
////  mRenderScene->add(mRship);
//  mRenderScene->add(mRsphere);
//  mRenderScene->add(mEmitter->renderable());
////  mRenderScene->add(mLight1);
//  mRenderScene->add(mLight2);
//  mRenderScene->add(mLight3);
////  mRenderScene->add(*g_theUiCamera);
  // mRenderScene->add(*mDebugCamera);
  mRenderScene->add(*mCamera);
  mMap.addToScene(*mRenderScene);
  // mRenderScene->add(mPlayer->renderable);
  // mRenderScene->add(mPlayer->turret.renderable);
  mRenderScene->add(mSun);
  //mCamera->handlePrePass([this](const Camera& cam) {
  //});
  mCamera->transfrom().localTranslate({ 0, .4f, -3.f });

  Debug::drawGrid(vec3::zero, vec3::right, vec3::forward, 1.f, 50.f, Debug::INF, Rgba::black);
  Debug::drawBasis();
}

Level::~Level() {
  delete mCamera;
  delete mGameClock;
}

void Level::loadResources() {
}

void Level::update(float dsec) {
  mCamera->update();
  mSun.transform.setlocalTransform(
    mat44::lookAt(
      vec3{ -5.f ,10.f, -5.f } + mPlayer->transform.position(), 
      vec3(10, 0, 10) + mPlayer->transform.position()));

  mSun.asDirectionalLight(3.f, vec3(1, 0, 0));
  mSun.transform.setlocalTransform(
    mat44::lookAt(
    vec3{ -5.f ,10.f, -5.f },
    vec3(10, 0, 10)));
  mMap.update();
  ray3 ray = { mCamera->transfrom().position(), mCamera->transfrom().forward() };
  contact3 c = mMap.raycast(ray);

  float distance2 = c.valid() ? mCamera->transfrom().position().distance2(c.position): INFINITY;

  for(Enemy* e: mEnemies) {
    contact3 cc = ray.intersect(e->bound());

    if (!cc.valid()) continue;

    float newDistance2 = mCamera->transfrom().position().distance2(cc.position);
    if(newDistance2 <= distance2) {
      distance2 = newDistance2;
      c = cc;
      Debug::log(Stringf("Hit enemy at: %s", cc.position.toString().c_str()), Rgba::red);
    }
  }

  for (EnemySpawner* e : mEnemySpawners) {
    contact3 cc = ray.intersect(e->bound());

    if (!cc.valid()) continue;

    float newDistance2 = mCamera->transfrom().position().distance2(cc.position);
    if (newDistance2 <= distance2) {
      distance2 = newDistance2;
      c = cc;
      Debug::log(Stringf("Hit Spawner at: %s", cc.position.toString().c_str()), Rgba::red);

    }
  }

  Debug::drawCube(c.position, .05f, false, 0, Rgba::red);
  if(c.valid()) {
    mPlayer->turnToward(c.position, 180.f * dsec);
  }
  mPlayer->update(dsec);
  // Debug::drawCube(mMap.bound().center(), mMap.bound().size(), true, 0);
  for(Entity* e: mEntities) {
    e->update(dsec);
  }

  for(Enemy* e: mEnemies) {
    float height = mMap.height(e->transform.position().xz());
    EXPECTS(height <= 10.f && height >= -10.f);
    e->transform.localPosition().y = height;
  }

  for(EnemySpawner* e: mEnemySpawners) {
    if(e->interval.decrement()) {
      if (mEnemies.size() >= 100.f) continue;
      addEntity<Enemy>(e->transform.position());
    }
  }

  float height = mMap.height(mPlayer->transform.position().xz());
  mPlayer->transform.localPosition().y = height;

  for (size_t i = mBullets.size() - 1; i < mBullets.size(); --i) {

    Bullet& b = *mBullets[i];

    if(b.transform.position().y < mMap.height(b.transform.position().xz())) {
      b.mIsDead = true;
      continue;
    }
    for (size_t j = mEnemies.size() - 1; j < mEnemies.size(); --j) {
      Enemy* e = mEnemies[j];
      constexpr float size = (Bullet::BULLET_SIZE + Enemy::ENEMY_SIZE);
      if (b.transform.position().distance2(e->transform.position()) < size * size) {
        b.mIsDead = true;
        e->mIsDead = true;
      }
    }

    for (size_t j = mEnemySpawners.size() - 1; j < mEnemySpawners.size(); --j) {
      EnemySpawner* e = mEnemySpawners[j];
      if (e->bound().contains(b.transform.position())) {
        b.mIsDead = true;
        e->mIsDead = true;
      }
    }
  }

  for(Enemy* e: mEnemies) {
    if(mPlayer->transform.position().distance2(e->transform.position()) < Enemy::ENEMY_SIZE * Enemy::ENEMY_SIZE) {
      mPlayer->cutHealth(1);
      e->mIsDead = true;
    }
  }
}



void Level::render() const {
  static ForwardRendering fw(g_theRenderer);
  g_theRenderer->setAmbient({ 1,1,1, 0.f });

  // aabb3 bound(vec3::zero, 2.f * vec3::one);

  // Debug::drawCube(bound, (mCamera->projection()*mCamera->view()).inverse(), true, 0);
  fw.render(*mRenderScene);


  mHud.render();
  Debug::drawNow();

}

void Level::processInput(float dSecond) {
  static int frameCount = 0;

  if(frameCount++ < 10) return;
  static vec3 position(10.f, 10.f, 10.f);

  vec2 mouseDelta = g_theInput->mouseDeltaPosition(false);

  vec3 d(mouseDelta.y, mouseDelta.x, 0);

  d *= 2.f;
  Debug::log(d.toString());
  if(d.magnitude() > 0) {
    //    d = d.normalized();
    d.x *= dSecond * 8.f;
    d.y *= dSecond * 8.f;
  }

  if(g_theInput->isKeyJustDown('P')) {
    addEntity<Enemy>(mPlayer->transform.position() + mPlayer->transform.forward() * 10.f);
  }

  // mPlayer->transform.localRotate({0, d.y, 0});



  if(g_theInput->isKeyJustDown('G')) {
    mPlayer->shoot();
  }

  // if(g_theInput->isKeyJustDown(KEYBOARD_F1)) {
  //   Debug::setCamera(mDebugCamera);
  //   mRenderScene->add(*mDebugCamera);
  // }

  // if(g_theInput->isKeyJustUp(KEYBOARD_F1)) {
  //   Debug::setCamera(mCamera);
  //   mRenderScene->remove(*mDebugCamera);
  // }

  if(!g_theInput->isKeyDown(KEYBOARD_F1)) {
    mCamera->processInput(dSecond);
    // vec3 dposition = mPlayer->transform.position();
    mPlayer->onInput(dSecond);
    // dposition = mPlayer->transform.position() - dposition;
    // mSun.transform.localTranslate(dposition);

  } else {
    mDebugCamera->transfrom().localRotate({d.x, d.y, 0});
    d = vec3::zero;
    if(g_theInput->isKeyDown('W')) {
      d += mDebugCamera->forward() * dSecond * 5.f;
    }
    if(g_theInput->isKeyDown('S')) {
      d -= mDebugCamera->forward() * dSecond * 5.f;
    }
    if(g_theInput->isKeyDown('A')) {
      d -= mDebugCamera->right() * dSecond * 5.f;
    }
    if(g_theInput->isKeyDown('D')) {
      d += mDebugCamera->right() * dSecond * 5.f;
    }
    mDebugCamera->transfrom().localTranslate(d);
  }

  if(g_theInput->isKeyJustDown(KEYBOARD_F5)) {
    Resource<Shader>::reload();
  }

}

void Level::swam(Enemy& e) {
  if (mEnemies.size() == 1) return;
  float factorTarget = 0.13f * 2.f;
  float factorGroup = 0.33f * 1.f;
  float factorToEscape = 0.54f * 2.f;

  float escapeRange2 = 1.f;
  vec3 groupVelocity = vec3::zero;
  vec3 escapeVelocity = vec3::zero;

  vec3 centerPosition;
  vec3 currentPosition = e.transform.position();
  for(Enemy* ee: mEnemies) {
    if (ee == &e) continue;
    vec3 position = ee->transform.position();
    centerPosition += position;

    if(position.distance2(currentPosition) < escapeRange2) {
      escapeVelocity += (currentPosition - position).normalized();
    }
  }

  if(!equal0(escapeVelocity.magnitude2())) {
    escapeVelocity.normalize();
    escapeVelocity *= factorToEscape;
  }

  size_t enemyCount = mEnemies.size() - 1u;
  if (enemyCount != 0) {
    centerPosition /= (float)enemyCount;
    groupVelocity = (centerPosition - currentPosition).normalized() * factorGroup;
  }

  e.velocity = e.velocity * factorTarget + escapeVelocity * factorToEscape + groupVelocity * factorGroup;
}

bool Level::isPlayerWin() {
  return mEnemies.size() == 0 && mEnemySpawners.size() == 0;
}

Level& Level::currentLevel() {
  return *gCurrentLevel;
}

void Level::afterFrame() {
   for(size_t i = mBullets.size() - 1; i < mBullets.size(); --i) {
  
     Bullet& b = *mBullets[i];
  
     if(b.isDead()) {
       removeEntity(b);
     }
   }

   for (size_t i = mEnemies.size() - 1; i < mEnemies.size(); --i) {
     Enemy* e = mEnemies[i];
     if (e->isDead()) removeEntity(*e);
     
   }

   for (size_t i = mEnemySpawners.size() - 1; i < mEnemySpawners.size(); --i) {
     EnemySpawner* e = mEnemySpawners[i];
     if (e->isDead()) removeEntity(*e);
   }
}

//Camera* createCamera() {
//  Camera* cam = new Camera();
//  cam->transfrom().rotation() = vec3::zero;
//  return cam;
//}
void Level::updateLight() {

}

void HUD::render() const {
  constexpr float HEALTH_BAR_LEN = 500.f;
  g_theRenderer->setCamera(g_theUiCamera);
  g_theRenderer->setMaterial(Resource<Material>::get("material/ui/default").get());

  // g_theRenderer->setModelMatrix(mat44::makeTranslation2D({ 10, 10 }));
  g_theRenderer->drawAABB2({ { 10.f, 20.f }, { 10+HEALTH_BAR_LEN, 50.f   } }, Rgba::black);
  g_theRenderer->drawAABB2({ { 10.f, 20.f }, { 10 + HEALTH_BAR_LEN * level.mPlayer->healthScaled(), 50.f } }, Rgba::red);

  g_theRenderer->drawText2(Stringf("  Enemy Left: %u", level.mEnemies.size()), 16.f, Font::Default().get());

  // g_theRenderer->setModelMatrix(mat44::identity);
}
