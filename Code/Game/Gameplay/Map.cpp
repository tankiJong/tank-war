#include "Map.hpp"
#include "Engine/Core/Resource.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Math/Noise/SmoothNoise.hpp"
#include "Engine/Math/Primitives/ivec2.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Debug/Log.hpp"

void Map::init() {
  renderable.transform() = &mTransform;
  renderable.material(Resource<Material>::get("material/couch"));

  Mesher ms;



  for(float j = 0; j < (float)countx; ++j) {
    for(float i = 0; i < (float)county; ++i) {
      ms.begin(DRAW_TRIANGES);
      ms.surfacePatch([i, j, this](const vec2& pos, const ivec2& index) {
        float height = Compute2dPerlinNoise(pos.x, pos.y, 4.f);

        if (index.x < 0 || index.y < 0) return vec3{ pos.x, height, pos.y };
        if (index.x >= countx || index.y >= countx) return vec3{ pos.x, height, pos.y };

        mHeight[indexOf(i, j, index.x, index.y)] = height;
        return vec3{ pos.x, height, pos.y };

      }, { i*rx , i*rx + rx }, { j*ry, j*ry + ry }, countx, county);
      ms.end();
    }
  }
  renderable.mesh() = ms.createMesh<vertex_lit_t>();
}

Map::~Map() {
  delete renderable.mesh();
  renderable.mesh() = nullptr;
}

uint Map::indexOf(uint x, uint y, uint offsetX, uint offsetY) {
  uint realOffsetX = x * countx + offsetX;
  uint realOffsetY = y * county + offsetY;

  return realOffsetY * countx * countx + realOffsetX;
}

float Map::height(vec2 pos) {
  float x = pos.x / rx;
  float y = pos.y / ry;

  uvec2 xy = uvec2{ uint(floorf(x)), uint(floorf(y)) };
  uvec2 offset =  uvec2((pos - vec2{ floorf(x) * rx, floorf(y) * ry }) / vec2 { rx / (float)countx, ry / (float)county });

  uint index = indexOf(xy.x, xy.y, offset.x, offset.y);
  Debug::log(Stringf("%u", index));
  return index < mHeight.size() ? mHeight[index] : 0.f;
}
