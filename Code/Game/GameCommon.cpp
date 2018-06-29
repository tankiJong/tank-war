#include "GameCommon.hpp"

const char* APP_NAME = "Proto - Tanki Zhang";

bool        g_isQuitting  = false;   // ...becomes App::m_isQuitting
Renderer*   g_theRenderer = nullptr;
Input*      g_theInput    = nullptr;
BitmapFont* g_defaultFont = nullptr;
Window*     g_theWindow   = nullptr;
Console*    g_theConsole  = nullptr;
Audio*      g_theAudio    = nullptr;
Camera*     g_theUiCamera = nullptr;