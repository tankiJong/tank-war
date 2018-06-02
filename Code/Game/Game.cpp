#include "Engine/Math/Primitives/vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Debug/Console/Console.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Renderer/Font.hpp"
#include "Engine/Renderer/Shader/Shader.hpp"
#include "Engine/Renderer/ForwardRendering.hpp"
#include "Engine/Debug/Log.hpp"

Game* g_theGame = nullptr;

Game::Game() {
  mGameClock = GetMainClock().createChild();

  mCamera = new Camera();
  mCamera->setProjectionPrespective(30.f, 5.f*CLIENT_ASPECT, 5.f, .1f, 100.f);
  mCamera->setColorTarget(g_theRenderer->getDefaultColorTarget());
  mCamera->setDepthStencilTarget(g_theRenderer->getDefaultDepthTarget());
  mCamera->setFlag(CAM_CLEAR_DEPTH | CAM_CLEAR_COLOR | CAM_EFFECT_BLOOM);
  Debug::setRenderer(g_theRenderer);
  Debug::setClock(&GetMainClock());
  Debug::setDuration(5.f);
  Debug::setCamera2D(g_theUiCamera);
  Debug::setCamera(mCamera);

  Debug::drawBasis({ 10, 10, 0 }, vec3::right * 100.f, vec3::up * 100.f, vec3::forward * 100.f);

  mCamera->transfrom().localRotation() = vec3::zero;
  mCamera->translate({ 3.f, 3.f, 3.f });
  mCamera->rotate({ -45.f, 135.f, 0.f });

  g_theUiCamera->sort = 1;
  mCamera->sort = 0;

  mRenderScene = new RenderScene();
////  mRenderScene->add(mRship);
//  mRenderScene->add(mRsphere);
//  mRenderScene->add(mEmitter->renderable());
////  mRenderScene->add(mLight1);
//  mRenderScene->add(mLight2);
//  mRenderScene->add(mLight3);
////  mRenderScene->add(*g_theUiCamera);
  mRenderScene->add(*mCamera);

  mCamera->handlePrePass([this](const Camera& cam) {
  });

}

Game::~Game() {
}

void Game::loadResources() {
  Debug::log("resource is loading...", Rgba::red, 10.f);
}

void Game::update(float dsec) {

}



void Game::render() const {
  static ForwardRendering fw(g_theRenderer);

  g_theRenderer->setUnifrom("SPECULAR_AMOUNT", 3.f);
  g_theRenderer->setUnifrom("SPECULAR_POWER", 10.f);

  fw.render(*mRenderScene);

  Debug::drawNow();

}

void Game::processInput(float dSecond) {
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

  mCamera->rotate({d.x, d.y, 0});
  d = vec3::zero;
  vec3 fwd = mCamera->forward();
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
  mCamera->translate(d);


  if(g_theInput->isKeyJustDown(KEYBOARD_F5)) {
    Resource<Shader>::reload();
  }

}

void Game::beforeFrame() {
  g_theRenderer->updateTime((float)mGameClock->frame.second, (float)GetMainClock().frame.second);
}

void Game::afterFrame() {
}

//Camera* createCamera() {
//  Camera* cam = new Camera();
//  cam->transfrom().rotation() = vec3::zero;
//  return cam;
//}
void Game::updateLight() {

}