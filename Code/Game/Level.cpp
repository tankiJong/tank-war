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

Level* gCurrentLevel = nullptr;

Level::Level(): mRenderScene(new RenderScene()), mPlayer() {
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
  Debug::setCamera(mCamera);

  mCamera->transfrom().localRotation() = vec3::zero;
  //mCamera->translate({ 3.f, 3.f, 3.f });
  //mCamera->rotate({ -45.f, 135.f, 0.f });

  g_theUiCamera->sort = 1;
  mCamera->sort = 0;

  loadResources();

  mPlayer = new Player();
  mPlayer->init();
  mCamera->attach(mPlayer->transform, 2.f);
  mMap.init();

  mSun.transform.setlocalTransform(mat44::lookAt(vec3{-10.f ,5.f, -10.f}, vec3::zero).inverse());
  Debug::drawCone(mSun.transform.position(), mSun.transform.forward(), 2.f, 10.f, 10);
  Debug::drawBasis(mSun.transform.position(), mSun.transform.right(), mSun.transform.up(), mSun.transform.forward());
  mSun.asDirectionalLight(2.f, { 1, 0, 0 });
  mSun.castShadow = true;

////  mRenderScene->add(mRship);
//  mRenderScene->add(mRsphere);
//  mRenderScene->add(mEmitter->renderable());
////  mRenderScene->add(mLight1);
//  mRenderScene->add(mLight2);
//  mRenderScene->add(mLight3);
////  mRenderScene->add(*g_theUiCamera);
  mRenderScene->add(*mCamera);
  mRenderScene->add(mMap.renderable);
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
  //Debug::log("resource is loading...", Rgba::red, 10.f);

  //Resource<Texture>::define("/image/couch/couch_diffuse.png");
  //Resource<Texture>::define("/image/couch/couch_normal.png");


  //FileSystem::Get().foreach("/data", [](const fs::path& p, auto...) {
  //  if (p.extension() == ".shader") {
  //    Resource<Shader>::define(p.generic_string());
  //    return;
  //  }
  //});
  //FileSystem::Get().foreach("/data", [](const fs::path& p, auto...) {
  //  if (p.extension() == ".mat") {
  //    Resource<Material>::define(p.generic_string());
  //    return;
  //  }
  //});
  //Debug::log("resource is loaded...", Rgba::green, 15.f);
}

void Level::update(float) {
  // mCamera->update();
}



void Level::render() const {
  static ForwardRendering fw(g_theRenderer);
  g_theRenderer->setAmbient({ 1,1,1, .2f });

  fw.render(*mRenderScene);

  Debug::drawNow();

}

void Level::processInput(float dSecond) {
  static int frameCount = 0;
  
  if (frameCount++ < 10) return;
  static vec3 position(10.f, 10.f, 10.f);

   vec2 mouseDelta = g_theInput->mouseDeltaPosition(false);

   vec3 d(mouseDelta.y, mouseDelta.x, 0);

   if(d.magnitude() > 0) {
 //    d = d.normalized();
     d.x *= dSecond * .8f;
     d.y *= dSecond * .8f;
   }


   mPlayer->transform.localRotate({d.x, 0, 0});

  if(g_theInput->isKeyDown('Q')) {
    mPlayer->turret.transform.localRotate({ 0, dSecond * 60.f, 0 });
  }

  if (g_theInput->isKeyDown('E')) {
    mPlayer->turret.transform.localRotate({ 0, dSecond * -60.f, 0 });
  }

  if(!g_theInput->isKeyDown(KEYBOARD_F1)) {
    mCamera->processInput(dSecond);

    d = vec3::zero;
    if (g_theInput->isKeyDown('W')) {
      d += mCamera->forward() * dSecond * 5.f;
    }
    if (g_theInput->isKeyDown('S')) {
      d -= mCamera->forward() * dSecond * 5.f;
    }
    if (g_theInput->isKeyDown('A')) {
      mPlayer->transform.localRotate({ 0, 30.f * dSecond, 0 });
    }
    if (g_theInput->isKeyDown('D')) {
      mPlayer->transform.localRotate({ 0, -30.f * dSecond, 0 });
    }

    mPlayer->transform.localTranslate(d);
    
  } else {
    mDebugCamera->transfrom().localRotate({ d.x, d.y, 0 });
    d = vec3::zero;
    if (g_theInput->isKeyDown('W')) {
      d += mCamera->forward() * dSecond * 5.f;
    }
    if (g_theInput->isKeyDown('S')) {
      d -= mCamera->forward() * dSecond * 5.f;
    }
    if (g_theInput->isKeyDown('A')) {
      d -= mCamera->right() * dSecond * 5.f;
    }
    if (g_theInput->isKeyDown('D')) {
      d += mCamera->right() * dSecond * 5.f;
    }
    mDebugCamera->transfrom().localTranslate(d);
  }

  float height = mMap.height(mPlayer->transform.position().xz());
  mPlayer->transform.localPosition().y = height;
  Debug::log(Stringf("player position: %s, height: %.6f", mPlayer->transform.position().xz().toString().c_str(), height), Rgba::white, 0);

  Debug::drawBasis(mPlayer->transform.position(), mPlayer->transform.right(), mPlayer->transform.up(), mPlayer->transform.forward(), 0);
  // Debug::drawPoint(mPlayer->transform.position(), 5.f, Gradient(Rgba::red, Rgba::blue));
  if(g_theInput->isKeyJustDown(KEYBOARD_F5)) {
    Resource<Shader>::reload();
  }

}

Level& Level::currentLevel() {
  return *gCurrentLevel;
}

//Camera* createCamera() {
//  Camera* cam = new Camera();
//  cam->transfrom().rotation() = vec3::zero;
//  return cam;
//}
void Level::updateLight() {

}