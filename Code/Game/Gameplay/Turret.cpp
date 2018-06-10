#include "Turret.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"

Turret::Turret() {
  Mesher ms;

  ms.begin(DRAW_TRIANGES)
    .sphere(vec3::zero, .08f)
    .cube(vec3::forward * .1f, { .03f, .03f, .15f })
    .end();

  renderable.mesh() = ms.createMesh<vertex_lit_t>();
}

Turret::~Turret() {
}
