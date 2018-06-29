#include "Map.hpp"
#include "Engine/Core/Resource.hpp"
#include "Engine/Renderer/Geometry/Mesher.hpp"
#include "Engine/Math/Noise/SmoothNoise.hpp"
#include "Engine/Math/Primitives/ivec2.hpp"
#include "Engine/Debug/Draw.hpp"
#include "Engine/Debug/Log.hpp"
#include "Engine/Math/Curves.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Math/Primitives/ray3.hpp"
#include "Engine/Physics/contact3.hpp"
#include "Engine/Core/Time/Clock.hpp"

MapChunk::MapChunk() {
  renderable.transform() = &transform;
  renderable.material(Resource<Material>::get("material/terrain/grass_rocky"));
  mHeight.fill(INFINITY);
  mNormal.fill(vec3{ INFINITY, INFINITY, INFINITY });
}



void MapChunk::init(vec2 bottomLeft) {

  Mesher ms;
  ms.begin(DRAW_TRIANGES);
  constexpr float eps = 0.001f;
  constexpr float cellx = rx / float(countx);
  constexpr float celly = ry / float(county);
  for(uint j = 0; j <= county; ++j) {
    for(uint i = 0; i <= countx; ++i) {
      vec2 pos = bottomLeft + vec2(float(i) * cellx, float(j) * celly);
      float height = Compute2dPerlinNoise(pos.x, pos.y, 4.f);

      ms.uv(pos);

      vec2 posdx1 = pos + vec2{ -eps, 0 };
      vec2 posdx2 = pos + vec2{ eps, 0 };
      float heightx1 = Compute2dPerlinNoise(posdx1.x, posdx1.y, 4.f);
      float heightx2 = Compute2dPerlinNoise(posdx2.x, posdx2.y, 4.f);

      vec3 tan = (vec3{ posdx2.x, heightx2, posdx2.y } -vec3{ posdx1.x, heightx1, posdx1.y }).normalized();
      ms.tangent(tan);

      vec2 posdy1 = pos + vec2{ 0, -eps};
      vec2 posdy2 = pos + vec2{ 0, eps };
      float heighty1 = Compute2dPerlinNoise(posdy1.x, posdy1.y, 4.f);
      float heighty2 = Compute2dPerlinNoise(posdy2.x, posdy2.y, 4.f);

      vec3 bitan = (vec3{ posdy2.x, heighty2, posdy2.y } -vec3{ posdy1.x, heighty1, posdy1.y }).normalized();

      vec3 norm = bitan.cross(tan);
      ms.normal(norm.normalized());

      mHeight[indexOf(i, j)] = height;
      mNormal[indexOf(i, j)] = norm;

      vec3 v = { pos.x, height, pos.y };
      ms.vertex3f(v);
      mBound.grow(v);
    }
  }

  for (uint j = 0; j < county; ++j) {
    for (uint i = 0; i < countx; ++i) {
      uint start = i + j * (countx+1);
      ms.quad(start, start + 1, start + countx + 2, start + countx + 1);
    }
  }
  ms.end();


  renderable.mesh() = ms.createMesh<vertex_lit_t>();
}

MapChunk::~MapChunk() {
  delete renderable.mesh();
  renderable.mesh() = nullptr;
}

void Map::init() {
  for(float   j = 0; j < (float)countx; ++j) {
    for(float i = 0; i < (float)county; ++i) {
      mChunks[indexOf((uint)i, (uint)j)].init({i * rx, j * ry});
      mBound.grow(mChunks[indexOf((uint)i, (uint)j)].bound());
    }
  }

  mRenderableWater.transform() = &mTransform;
  mRenderableWater.material(Resource<Material>::get("material/water"));
  updateWater();

  for(float   i = 0; i < 10.f; ++i) {
    for(float j = 0; j < 10.f; ++j) {
      vec2    pos(i * .5f, j * .5f);
      Debug::drawCube({pos.x, height(pos), pos.y}, .05f, true, Debug::INF);
    }
  }
}

void Map::addToScene(RenderScene& scene) {
  for(MapChunk& c: mChunks) {
    scene.add(c.renderable);
  }
  scene.add(mRenderableWater);
}

void Map::update() {
  updateWater();
}

contact3 Map::raycast(const ray3& ray) const {
  constexpr float STEP_SIZE = .01f;

  vec3 previous = ray.start;
  {
    float h = height(previous.xz());
    if (h > previous.y) return contact3();
    if (h == previous.y) return { ray.start, normal(previous.xz()) };
  }
  
  // Debug::log(Stringf("Bound contact position: %s", contact.position.toString().c_str()));
  // if (!contact.valid()) {}
  vec3 current;
  contact3 contact;

  // previous height > map height
  float dest = ray.step(ray.evaluate(1000));
  for(float i = STEP_SIZE; i < dest; i += STEP_SIZE) {
    current = ray.evaluate(i);
    float h = height(current.xz());
    if (h > current.y) break;
    previous = current;
  }

  bool b1 = previous.y >= height(previous.xz());
  bool b2 = current.y <= height(current.xz());

  if (b1 == false || b2 == false) return {};
  // if (b2 == false) return contact;

  // Debug::drawCube(current, .5f, false, 0, Rgba::red);
  // Debug::drawLine(vec3::one * 10.f, current, 1, 0, Rgba::cyan);
  vec3 interest = (previous + current) * .5f;

  while(abs(interest.y - height(interest.xz()) > 0.001f)) {
    float dh = interest.y - height(interest.xz());
    ((dh > 0) ? previous : current) = interest;
    interest = (previous + current) * .5f;
  };

  return { interest, normal(contact.position.xz()) };
}

float Map::height(vec2 pos) const {
  pos = clamp(pos, vec2::zero, { 255.f, 255.f });

  float x = pos.x / rx;
  float y = pos.y / ry;

  uvec2 xy = uvec2{ uint(floorf(x)), uint(floorf(y)) };
  vec2 offset =  (pos - vec2{ floorf(x) * rx, floorf(y) * ry }) / vec2 { rx / (float)countx, ry / (float)county };

  const MapChunk& chunk = mChunks[indexOf(xy.x, xy.y)];
  float bl = chunk.height(uint(floor(offset.x)), uint(floor(offset.y)));
  float br = chunk.height(uint(ceil(offset.x)), uint(floor(offset.y)));
  float tl = chunk.height(uint(floor(offset.x)), uint(ceil(offset.y)));
  float tr = chunk.height(uint(ceil(offset.x)), uint(ceil(offset.y)));

  float _;
  float hb = lerp(bl, br, modff(offset.x, &_));
  float ht = lerp(tl, tr, modff(offset.x, &_));

  float h = lerp(hb, ht, modff(offset.y, &_));
  
  // Debug::log(Stringf("%.5f", h));
  ENSURES(h < INFINITY);
  ENSURES(h != -NAN && h != NAN);
  return h;
}

vec3 Map::normal(vec2 pos) const {
  pos = clamp(pos, vec2::zero, { 255.f, 255.f });

  float x = pos.x / rx;
  float y = pos.y / ry;

  uvec2 xy = uvec2{ uint(floorf(x)), uint(floorf(y)) };
  vec2 offset = (pos - vec2{ floorf(x) * rx, floorf(y) * ry }) / vec2{ rx / (float)countx, ry / (float)county };

  const MapChunk& chunk = mChunks[indexOf(xy.x, xy.y)];
  vec3 bl = chunk.normal(uint(floor(offset.x)), uint(floor(offset.y)));
  vec3 br = chunk.normal(uint(ceil(offset.x)), uint(floor(offset.y)));
  vec3 tl = chunk.normal(uint(floor(offset.x)), uint(ceil(offset.y)));
  vec3 tr = chunk.normal(uint(ceil(offset.x)), uint(ceil(offset.y)));

  float _;
  vec3 hb = lerp(bl, br, modff(offset.x, &_));
  vec3 ht = lerp(tl, tr, modff(offset.x, &_));

  vec3 h = lerp(hb, ht, modff(offset.y, &_));

  // Debug::log(Stringf("%.5f", h.toString()));
  ENSURES(h.x < INFINITY);
  return h;
}

void Map::updateWater() {
  delete mRenderableWater.mesh();
  Mesher ms;

  vec2 offset = vec2((float)GetMainClock().total.second * .06f);
  ms.begin(DRAW_TRIANGES)
    .uv(vec2::zero + offset)
    .color(Rgba(255, 255, 255, 128))
    .vertex3f(vec3::zero);

  ms.uv(vec2(MapSize.x, 0) + offset)
    .color(Rgba(255, 255, 255, 128))
    .vertex3f(vec3(MapSize.x, 0, 0));

  ms.uv(vec2(MapSize.x, MapSize.y) + offset)
    .color(Rgba(255, 255, 255, 128))
    .vertex3f(vec3(MapSize.x, 0, MapSize.y));

  ms.uv(vec2(0, MapSize.y) + offset)
    .vertex3f(vec3(0, 0, MapSize.y));

  ms.quad()
    .end();

  mRenderableWater.mesh() = ms.createMesh<vertex_lit_t>();
}
