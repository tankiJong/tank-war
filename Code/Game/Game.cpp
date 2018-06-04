﻿#include "Game.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Renderer/Font.hpp"
#include "Engine/Application/Window.hpp"
#include "Engine/Debug/Draw.hpp"
#include <chrono>
#include <thread>
#include "Engine/Debug/Log.hpp"
#include "Engine/File/FileSystem.hpp"
#include "Engine/Renderer/ForwardRendering.hpp"
#include "Engine/Renderer/Shader/Shader.hpp"
#include "Engine/Renderer/Shader/Material.hpp"
Transform uiTransform;
Game::Game() {
  Debug::setRenderer(g_theRenderer);
  Debug::setClock(&GetMainClock());
  Debug::setDuration(5.f);
  Debug::setCamera2D(g_theUiCamera);
  loadResources();

  mUis[STATE_LOADING].init("loading assets"sv, Rgba::cyan);
  mUis[STATE_MAIN_MENU].init("Hit SPACE to start"sv, Rgba::black);
  mUis[STATE_READY_UP].init("Hit Enter when you are ready"sv, Rgba::green);
  mRenderScene = new RenderScene();
  mRenderScene->add(*g_theUiCamera);
  mUis[state].regScene(*mRenderScene);
}

void Game::beforeFrame() {
}

void Game::afterFrame() {}

void Game::render() const {
  static uint frameCount = 0;
  frameCount++;

  if(state == STATE_LEVEL) {
    mLevel->render();
    return;
  }
  ForwardRendering fw(g_theRenderer);
  fw.render(*mRenderScene);
}

void Game::loadResources() const {
  Debug::log("resource is loading...", Rgba::red, 10.f);

  Resource<Texture>::define("/image/couch/couch_diffuse.png");
  Resource<Texture>::define("/image/couch/couch_normal.png");


  FileSystem::Get().foreach("/data", [](const fs::path& p, auto...) {
    if (p.extension() == ".shader") {
      Resource<Shader>::define(p.generic_string());
      return;
    }
  });
  FileSystem::Get().foreach("/data", [](const fs::path& p, auto...) {
    if (p.extension() == ".mat") {
      Resource<Material>::define(p.generic_string());
      return;
    }
  });
  Debug::log("resource is loaded...", Rgba::green, 15.f);
}

void Game::update(float dSecond) {
  static bool justSwitch = false;
  switch(state) {
    case STATE_LOADING: {
      if(GetMainClock().total.second > 3.0) {
        switchState(STATE_MAIN_MENU);
      }
    }; break;
    case STATE_MAIN_MENU: break;
    case STATE_READY_UP: break;
    case STATE_LEVEL: 
      mLevel->update(dSecond);
    break;
    default: ;
  }
}

void Game::processInput(float dSecond) {
  switch(state) { 
    case STATE_LOADING: break;
    case STATE_MAIN_MENU:
      if(g_theInput->isKeyJustDown(KEYBOARD_SPACE)) {
        EXPECTS(mLevel == nullptr);
        mLevel = new Level();
        switchState(STATE_READY_UP);
      }
    break;
    case STATE_READY_UP: 
      if (g_theInput->isKeyJustDown(KEYBOARD_RETURN)) {
        switchState(STATE_LEVEL);
      }
    break;
    case STATE_LEVEL:
      if(mLevel) {
        mLevel->processInput(dSecond);
      }
    break;
    default: ;
  }
}

void Game::switchState(eGameState to) {
  mUis[state].unregScene(*mRenderScene);
  state = to;
  mUis[state].regScene(*mRenderScene);
}

void SceenWord::init(std::string_view txt, const Rgba& color) {
  text.transform()       = &transform;
  background.transform() = &transform;

  text.material(Resource<Material>::get("material/ui/font"));
  text.material()->setTexture(TEXTURE_DIFFUSE, Font::Default()->texture());
  background.material(Resource<Material>::get("material/ui/default"));

  Mesher ms;
  vec2   screen = (vec2)Window::Get()->bounds().size();
  vec2   span   = (screen - vec2{Font::Default()->advance(txt, 20.f), Font::Default()->lineHeight(20.f)}) / 2.f;
  ms.begin(DRAW_TRIANGES)
    .text(txt, 20.f, Font::Default().get(), vec3{span, 0})
    .end();
  text.mesh() = ms.createMesh<>();

  ms.clear();

  ms.begin(DRAW_TRIANGES)
    .color(color)
    .quad2({vec2::zero, screen}, 1.f)
    .end();
  background.mesh() = ms.createMesh<>();
}
