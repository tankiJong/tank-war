#include "Player.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Input/Input.hpp"
#include "Game/Level.hpp"
#include "util.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Debug/Log.hpp"

void Player::update(float) {
  Debug::drawLine(turret.transform.position(), turret.transform.position() + turret.transform.forward() * 10.f, 1.f, 0, Rgba::yellow);
}

void Player::onInput(float dSecond) {
  vec3 d;
  Euler rot = transform.localRotation();
  vec3 position = transform.position();

  vec3 up = Level::currentLevel().map().normal(transform.position().xz()).normalized();
  vec3 forward = transform.right().cross(up).normalized();
  vec3 right = up.cross(forward);

  transform.setWorldTransform(mat44(right, up, forward, position));

  if (g_theInput->isKeyDown('W')) {
    d += transform.forward() * dSecond * 5.f;
  }
  if (g_theInput->isKeyDown('S')) {
    d -= transform.forward() * dSecond * 5.f;
  }
  if (g_theInput->isKeyDown('A')) {
    transform.localRotate({ 0, 60.f * dSecond, 0 });
  }
  if (g_theInput->isKeyDown('D')) {
    transform.localRotate({ 0, -60.f * dSecond, 0 });
  }

  transform.localTranslate(d);

  if (g_theInput->isKeyDown('Q')) {
    transform.localRotate({ 0, dSecond * 60.f, 0 });
  }

  if (g_theInput->isKeyDown('E')) {
    transform.localRotate({ 0, dSecond * -60.f, 0 });
  }

}

Player::~Player() {

}
