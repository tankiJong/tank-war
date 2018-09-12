#include "Game.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Renderer/Font.hpp"
#include "Engine/Application/Window.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Debug/Log.hpp"
#include "Engine/File/FileSystem.hpp"
#include "Engine/Renderer/ForwardRendering.hpp"
#include "Engine/Renderer/Shader/Shader.hpp"
#include "Engine/Renderer/Shader/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Gameplay/Enemy.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Debug/Profile/Profiler.hpp"
#include "Engine/Debug/Profile/Overlay.hpp"
#include "Engine/Async/Thread.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Debug/Console/Command.hpp"

// class test
#define GAME_PORT 10084
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

// IPv4 Header Size: 20B
// IPv6 Header Size: 40B
// TCP Headre Size: 20B-60B
// UDP Header Size: 8B 
// Ethernet: 28B, but MTU is already adjusted for it
// so packet size is 1500 - 40 - 8 => 1452B (why?)

class UDPTest {
public:

  bool start() {
    // get an address to use; 
    NetAddress addr = "10.8.147.162:10084";

    if (!m_socket.bind(addr, 10)) {
      Console::error("Failed to bind.");
      return false;
    } else {
      m_socket.setOption(~SOCKET_OPTION_BLOCKING); // if you have cached options, you could do this
                                    // in the constructor; 
      Console::info(Stringf("Socket bound: %s", m_socket.address().toString()));
      ENSURES(m_socket.valid());
      return true;
    }
  }

  void stop() {
    m_socket.close();
  }

  void send_to(NetAddress const &addr, void const *buffer, uint byte_count) {
    m_socket.send(addr, buffer, byte_count);
  }

  void update() {
    byte_t buffer[PACKET_MTU];

    NetAddress from_addr;
    size_t read = m_socket.receive(from_addr, buffer, PACKET_MTU);

    if (read > 0U) {
      uint max_bytes = std::min<uint>(read, 128u);
      std::string output = "0x";
      output.resize(max_bytes * 2U + 3U, 0);
      char* iter = output.data();

      iter += 2U; // skip the 0x
      for (uint i = 0; i < read; ++i) {
        sprintf_s(iter, max_bytes * 2U + 3U, "%02X", buffer[i]);
        iter += 2U;
      }
      *iter = NULL;

      Console::info(Stringf("Received from %s;\n%s", from_addr.toString(),
                    output.c_str()));
    }
  }

public:
  // if you have multiple address, you can use multiple sockets
  // but you have to be clear on which one you're sending from; 
  UDPSocket m_socket;
};






Transform uiTransform;

SoundID gBgm;
SoundPlaybackID gBgmPlayback;
SoundID gMenuSelect;

SoundID gMainGameBgm;


UDPTest* udpTest = nullptr;
Game::Game() {
  Debug::setRenderer(g_theRenderer);
  Debug::setClock(&GetMainClock());
  Debug::setDuration(5.f);
  Debug::setCamera2D(g_theUiCamera);
  loadResources();

  mUis[STATE_LOADING].init("loading assets"sv, Rgba::cyan);
  mUis[STATE_MAIN_MENU].init("Hit SPACE to start"sv, Rgba::black);
  mUis[STATE_READY_UP].init("Hit Enter when you are ready"sv, Rgba::green);
  mUis[STATE_DIE_PAUSE].init("You DIED. Hit SPACE to continue..."sv, Rgba::red);
  mUis[STATE_WIN].init("You win."sv, Rgba::blue);

  g_theInput->mouseLockCursor(true);

  SAFE_DELETE(udpTest);
  udpTest = new UDPTest();
}

void Game::beforeFrame() {
  // Log::tagf("AAA", "i am log from tag AAA");
  // Log::tagf("BBB", "i am log from tag BBB");
  // Log::tagf("CCC", "i am log from tag CCC");
  // Log::tagf("DDD", "i am log from tag DDD");
}

void Game::afterFrame() {
  if(state == STATE_LEVEL) {
    mLevel->afterFrame();
  }
}

void Game::render() const {
  static uint frameCount = 0;
  frameCount++;
  g_theRenderer->cleanScreen(Rgba::black);

  if(state == STATE_LEVEL) {
    mLevel->render();
  } else {
    mUis[state].render();
  }

  if (g_theInput->isKeyJustDown('0')) {
    g_theRenderer->screenShot("screenshot.bmp");
  }

}

void Game::loadResources() const {
  FileSystem::Get().foreach("/data", [](const fs::path& p, auto...) {
    if (p.extension() == ".png" || p.extension() == ".jpg") {
      Resource<Texture>::define(p.generic_string());
      return;
    }
  });


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

  gBgm = g_theAudio->createOrGetSound("Data/audio/music/mainmenu.mp3");
  gMainGameBgm = g_theAudio->createOrGetSound("Data/audio/music/world.mp3");
  gMenuSelect = g_theAudio->createOrGetSound("Data/audio/sfx/menu.select.wav");
}

void Game::update(float dSecond) {
  static bool justSwitch = false;

  udpTest->update();
  switch(state) {
    case STATE_LOADING: {
      switchState(STATE_MAIN_MENU);
      gBgmPlayback = g_theAudio->playSound(gBgm);

    }; break;
    case STATE_MAIN_MENU: break;
    case STATE_READY_UP: break;
    case STATE_LEVEL:
      mLevel->update(dSecond);
      if(mLevel->isPlayerWin()) {
        switchState(STATE_WIN);
      }
    break;
    case STATE_WIN: break;
    default: ;
  }

}

void Game::processInput(float dSecond) {
  PROF_FUNC();

  switch(state) { 
    case STATE_LOADING: break;
    case STATE_MAIN_MENU:
      if(g_theInput->isKeyJustDown(KEYBOARD_SPACE)) {
        g_theAudio->playSound(gMenuSelect);
        EXPECTS(mLevel == nullptr);
        mLevel = new Level();
        switchState(STATE_READY_UP);
      }
    break;
    case STATE_READY_UP: 
      if (g_theInput->isKeyJustDown(KEYBOARD_RETURN)) {
        g_theAudio->playSound(gMenuSelect);
        switchState(STATE_LEVEL);
        g_theAudio->stopSound(gBgmPlayback);
        g_theAudio->playSound(gMainGameBgm);
      }
    break;
    case STATE_LEVEL:
      if(mLevel->mPlayer->isDead()) {
        switchState(STATE_DIE_PAUSE);
      }
      if(mLevel) {
        mLevel->processInput(dSecond);
      }
    break;
    case STATE_DIE_PAUSE:
      if(g_theInput->isKeyJustDown(KEYBOARD_SPACE)) {
        mLevel->mPlayer->recovery();
        aabb2 box;
        for(Enemy* enemy: mLevel->mEnemies) {
          box.stretchToIncludePoint(enemy->transform.position().xz());
        }
        box.addPaddingToSides(10.f, 10.f);
        aabb2 box2 = box;
        box.mins = clamp(box.mins, vec2::zero, vec2(MapSize.x, MapSize.y));
        box.maxs = clamp(box.maxs, vec2::zero, vec2(MapSize.x, MapSize.y));
        vec2 pos = (box2.maxs == box.maxs) ? box2.maxs : box2.mins;
        mLevel->mPlayer->transform.localPosition() = vec3(pos.x, 0, pos.y);
        switchState(STATE_LEVEL);
      }
    default: ;
  }
}

void Game::switchState(eGameState to) {
  state = to;
}

void SceenWord::init(std::string_view txt, const Rgba& color) {
  func = [txt, color]() {
    vec2 screen = (vec2)Window::Get()->bounds().size();
    vec2   span = (screen - vec2{ Font::Default()->advance(txt, 20.f), Font::Default()->lineHeight(20.f) }) / 2.f;
    g_theRenderer->setCamera(g_theUiCamera);
    g_theRenderer->cleanScreen(Rgba::black);
    g_theRenderer->disableDepth();
    g_theRenderer->setTexture();
    g_theRenderer->setModelMatrix(mat44::identity);
    g_theRenderer->drawAABB2({ vec2::zero, screen }, color);
    g_theRenderer->drawText2(txt, 20.f, Font::Default().get(), vec3{ span, 0 });
  };
}



COMMAND_REG("udp_test_start", "", "start udp test")(Command& cmd) {
  udpTest->start();
  return true;
}

COMMAND_REG("udp_test_stop", "", "stop udp test")(Command& cmd) {
  udpTest->stop();

  return true;
}

COMMAND_REG("udp_test_send", "addr str", "Send a message")(Command& cmd) {

  std::string str = cmd.arg<0>();

  NetAddress addr = str;

  std::string content = cmd.arg<1>();

  Console::info("Sending message...");

  udpTest->send_to(addr, content.c_str(), content.size());

  return true;
}