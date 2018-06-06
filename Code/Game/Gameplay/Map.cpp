#include "Map.hpp"
#include "Engine/Core/Resource.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Math/Noise/SmoothNoise.hpp"
#include "Engine/Math/Primitives/ivec2.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Debug/Log.hpp"
#include "Engine/Math/Curves.hpp"

void Map::init() {
  renderable.transform() = &mTransform;
  renderable.material(Resource<Material>::get("material/couch"));

  Mesher ms;

  mHeight.fill(INFINITY);

  for(float j = 0; j < (float)countx; ++j) {
    for(float i = 0; i < (float)county; ++i) {
      ms.begin(DRAW_TRIANGES);
      ms.surfacePatch([i, j, this](const vec2& pos, const ivec2& index) {
        float height = Compute2dPerlinNoise(pos.x, pos.y, 4.f);

        ivec2 clampIdx = clamp(index, ivec2::zero, ivec2{ (int)countx - 1, (int)county - 1 });

        EXPECTS(indexOf((uint)i, (uint)j, clampIdx.x, clampIdx.y) < mHeight.size());
        if(mHeight[indexOf((uint)i, (uint)j, clampIdx.x, clampIdx.y)] == INFINITY) {
          mHeight[indexOf((uint)i, (uint)j, clampIdx.x, clampIdx.y)] = height;
        }
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
  pos = clamp(pos, vec2::zero, { 256.f, 356.f });

  float x = pos.x / rx;
  float y = pos.y / ry;

  uvec2 xy = uvec2{ uint(floorf(x)), uint(floorf(y)) };
  vec2 offset =  (pos - vec2{ floorf(x) * rx, floorf(y) * ry }) / vec2 { rx / (float)countx, ry / (float)county };

  float bl = mHeight[indexOf(xy.x, xy.y, uint(floor(offset.x)), uint(floor(offset.y)))];
  float br = mHeight[indexOf(xy.x, xy.y, uint(ceil(offset.x)), uint(floor(offset.y)))];
  float tl = mHeight[indexOf(xy.x, xy.y, uint(floor(offset.x)), uint(ceil(offset.y)))];
  float tr = mHeight[indexOf(xy.x, xy.y, uint(ceil(offset.x)), uint(ceil(offset.y)))];

  float _;
  float hb = lerp(bl, br, modff(offset.x, &_));
  float ht = lerp(tl, tr, modff(offset.x, &_));

  float h = lerp(hb, ht, modff(offset.y, &_));
  
  Debug::log(Stringf("%.5f", h));
  ENSURES(h < INFINITY);
  return h;
}
