#include "Tank.hpp"
#include "Engine/Renderer/Geometry/Mesh.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Game/Level.hpp"
#include "Game/Gameplay/Bullet.hpp"
#include "Game/Gameplay/util.hpp"
#include "Engine/Audio/Audio.hpp"
SoundID gShootsfx;
void Tank::init() {
  Mesher ms;

  ms.begin(DRAW_TRIANGES)
    .cube(vec3::up * .04f, { .2f, .08f, .4f })
    .end();

  renderable.mesh() = ms.createMesh<vertex_lit_t>();

  //transform.localTranslate(vec3(0, 0, 5.f));
  turret.transform.parent() = &transform;
  turret.transform.localTranslate(vec3::up * .13f);
  transform.localRotate({ 0, 90.f, 0 });

  gShootsfx = g_theAudio->createOrGetSound("Data/audio/sfx/Gun_Pistol_Shot01.wav");
}

void Tank::turn(float degSec) {
  transform.localRotate({ 0, degSec * (float)GetMainClock().frame.second, 0 });
}

void Tank::turnToward(const vec3& target, float speedDeg) {
  ::turnToward(turret.transform, target, speedDeg);
}

void Tank::advance(float speedSec) {
  transform.localTranslate(transform.forward() * speedSec * (float)GetMainClock().frame.second);
}

void Tank::advance(float speedSec, const vec3 norm) {
  transform.localTranslate(transform.forward() * speedSec * (float)GetMainClock().frame.second);

  vec3 up = norm.normalized();

  vec3 currentUp = transform.up().normalized();

  vec3 right = currentUp.cross(transform.forward()).normalized();
  vec3 forward = right.cross(up);

  mat44 worldToCurrent = mat44(right, up, forward, transform.position());

  transform.setWorldTransform(worldToCurrent);
}

void Tank::turnTurret(float degSec) {
  turret.transform.localRotate({ 0, (float)GetMainClock().frame.second * degSec, 0 });
}

void Tank::shoot() {
  vec3 startPosition = turret.transform.position() + turret.transform.forward() * .5f;
  Bullet* b = Level::currentLevel().addEntity<Bullet>(startPosition);
  b->velocity = turret.transform.forward() * 6.f;
  g_theAudio->playSound(gShootsfx);
}
