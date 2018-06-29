#include "EnemySpawner.hpp"
#include "Engine/Renderer/Geometry/Vertex.hpp"
#include "Game/Gameplay/Enemy.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Math/Primitives/ray3.hpp"
#include "Game/Level.hpp"
#include "Engine/Debug/Draw.hpp"

void EnemySpawner::init() {
  Mesher ms;

  ms.begin(DRAW_TRIANGES)
    .cube(vec3(0, 5.f, 0.f), { 3.f, 10.f, 3.f })
    .end();

  renderable.mesh() = ms.createMesh<vertex_lit_t>();
  mBound = aabb3({ 0, 5.f, 0.f }, { 3.f, 10.f, 3.f });
  interval.duration = 1.f;
}

void EnemySpawner::update(float) {
}

aabb3 EnemySpawner::bound() const {
  aabb3 bounds;
  bounds.min = transform.position() + mBound.min;
  bounds.max = transform.position() + mBound.max;
  return bounds;
}

owner<Enemy*> EnemySpawner::spawn() const {
  Enemy* e = new Enemy();
  e->init();
  e->transform = transform;

  return e;
}
