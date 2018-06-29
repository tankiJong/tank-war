#include "Bullet.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Audio/Audio.hpp"

SoundID gexploSfx;
void Bullet::init() {
  Mesher ms;

  ms.begin(DRAW_TRIANGES)
    .sphere(vec3::zero, BULLET_SIZE)
    .end();

  renderable.mesh() = ms.createMesh<vertex_lit_t>();

  life.duration = MAX_AGE;
  life.flush();

  gexploSfx = g_theAudio->createOrGetSound("Data/audio/sfx/Hit_GunButt_PistolWhip1.wav");

}

void Bullet::update(float dSecond) {
  transform.localTranslate(velocity * dSecond);
  mIsDead = life.decrement();
}

sphere Bullet::bound() const {
  return { transform.position(), BULLET_SIZE };
}

Bullet::~Bullet() {
  g_theAudio->playSound(gexploSfx);
}
