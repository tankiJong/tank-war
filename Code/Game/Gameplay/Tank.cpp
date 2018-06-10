#include "Tank.hpp"
#include "Engine/Renderer/Geometry/Mesh.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"

void Tank::init() {
  Mesher ms;

  ms.begin(DRAW_TRIANGES)
    .cube(vec3::up * .04f, { .2f, .08f, .4f })
    .end();

  renderable.mesh() = ms.createMesh<vertex_lit_t>();

  //transform.localTranslate(vec3(0, 0, 5.f));
  turret.transform.parent() = &transform;
  turret.transform.localTranslate(vec3::up * .13f);
}

Tank::~Tank() {

}
