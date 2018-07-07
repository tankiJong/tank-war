#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Debug/Console/Console.hpp"
#include "Engine/Application/Window.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Renderer/Font.hpp"
#include "Engine/File/FileSystem.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Debug/Profile/Profiler.hpp"
#include "Engine/Debug/Profile/Overlay.hpp"
App* g_theApp = nullptr;

App::App() {
  FileSystem::Get().mount("/data", "Data");
  FileSystem::Get().mount("/shader", "Data/shader");
  FileSystem::Get().mount("/image", "Data/Images");

  Font::Default() = Resource<Font>::get("Fira Mono");
  g_theInput = &Input::Get();
  g_theConsole = Console::Get();
  g_theAudio = new Audio();
  g_theConsole->init(*g_theRenderer, *g_theInput);
//  g_theRenderer = new Renderer();
//  g_theConsole->open();

  g_theUiCamera = new Camera();

  const aabb2& bounds = Window::Get()->bounds();
  g_theUiCamera->setProjection(mat44::makeOrtho(0, bounds.width(), 0, bounds.height(), -1.f, 1.f));
  g_theUiCamera->setColorTarget(g_theRenderer->getDefaultColorTarget());

  m_game = new Game();
}

App::~App() {
//	delete g_theRenderer;
//	g_theRenderer = nullptr;
  g_theConsole = nullptr;
  SAFE_DELETE(g_theAudio);
 
  TODO("clean up console, Console::destoryInstance");
	delete g_theInput;
	g_theInput = nullptr;
    
	delete m_game;
  m_game = nullptr;

  delete g_theUiCamera;
  g_theUiCamera = nullptr;
}

void App::afterFrame() {
  m_game->afterFrame();
	g_theInput->afterFrame();
	g_theRenderer->afterFrame();
  g_theAudio->afterFrame();
}

void App::beforeFrame() {
  Profile::markFrame();
  g_theAudio->beforeFrame();
	g_theInput->beforeFrame();
	g_theRenderer->beforeFrame();
  GetMainClock().beginFrame();


  {
    PROF_SCOPE("A");
    {
      PROF_SCOPE("B")
    }
  }

  m_game->beforeFrame();
}

void App::update() {
  float dSecond = (float)GetMainClock().frame.second;

  if(!g_theConsole->opened()) {
    if (g_theInput->isKeyDown('T') || g_theInput->getController(XBOX_CONTROLLER_0)->isKeyDown(XBOX_KEY_LEFT_THUMB)) {
      m_game->update(dSecond / 10.f);
      return;
    }

    if(g_theInput->isKeyJustDown(KEYBOARD_ESCAPE)) {
      m_isQuitting = 1;
      return;
    }

    if(g_theInput->isKeyJustDown(KEYBOARD_OEM_3)) {
      g_theConsole->open();
    }

    m_game->processInput(dSecond);
  } else {
    g_theConsole->update(dSecond);
  } 
  m_game->update(dSecond);
  Profile::updateOverlay();

  // if(g_theInput->isKeyDown(MOUSE_LBUTTON)) {
  //   Profile::pause();
  // } else {
  //   Profile::resume();
  // }
}

void App::render() const {
//  g_theRenderer->cleanScreen(Rgba(0, 0, 0, 255));
  m_game->render();
  Debug::drawNow();
  Profile::renderOverlay();

  if(g_theConsole->isOpen()) {
    g_theConsole->render();
  }
}


