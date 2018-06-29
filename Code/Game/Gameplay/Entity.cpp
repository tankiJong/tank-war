#include "Entity.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Game/Level.hpp"

Entity::Entity() {
  Mesher ms;
  renderable.transform() = &transform;
  renderable.material(Resource<Material>::get("material/couch"));

  Level::currentLevel().renderScene().add(renderable);
}

Entity::~Entity() {
  Level::currentLevel().renderScene().remove(renderable);
  delete renderable.mesh();
  renderable.mesh() = nullptr;
}
