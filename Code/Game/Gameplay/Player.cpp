#include "Player.hpp"
#include "Engine/Renderer/Geometry/Mesh.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"

void Player::init() {
  Mesher ms;

  ms.begin(DRAW_TRIANGES)
    .sphere(vec3::zero, .1f)
    .end();

  renderable.mesh() = ms.createMesh<vertex_lit_t>();
  renderable.transform() = &transform;
  renderable.material(Resource<Material>::get("material/couch"));

  //transform.localTranslate(vec3(0, 0, 5.f));
}

Player::~Player() {
  delete renderable.mesh();
  renderable.mesh() = nullptr;
}
