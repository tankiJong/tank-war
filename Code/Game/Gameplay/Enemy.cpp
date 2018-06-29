#include "Enemy.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "util.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Debug/Log.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Level.hpp"

void Enemy::init() {
  Mesher ms;

  ms.begin(DRAW_TRIANGES)
    .sphere(vec3::zero, ENEMY_SIZE)
    .end();

  renderable.mesh() = ms.createMesh<vertex_lit_t>();

  transform.localRotate({ 0, 90.f, 0 });
}

void Enemy::update(float dSecond) {
  if(target != nullptr) {
    turnToward(transform, target->transform.position(), 180.f);
    EXPECTS(transform.localRotation().magnitude2() >= 0);
    velocity = transform.forward() * 1.f;
    // Debug::drawLine(transform.position(), target->transform.position(), 1, 0, Rgba::red, Rgba::red);
  }

  Level::currentLevel().swam(*this);

  turnToward(transform, transform.position() + velocity, 30.f*dSecond);

  transform.localTranslate(transform.forward()*velocity.magnitude()*dSecond);
  EXPECTS(transform.localRotation().magnitude2() >= 0);
  EXPECTS(-NAN != transform.localPosition().x);


  // Debug::drawBasis(transform, 0);
  // Debug::log(Stringf("Enemy position: %s", transform.position().toString().c_str()), Rgba::cyan);
}
