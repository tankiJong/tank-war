#pragma once
#include "Engine/Core/common.hpp"
#include "Engine/Renderer/Renderable/Renderable.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Curves.hpp"
#include "Game/Gameplay/util.hpp"
#include "Engine/Math/Primitives/aabb3.hpp"
#include "Engine/Physics/contact3.hpp"

struct ray3;

inline constexpr struct {
  float x = 256.f;
  float y = 256.f;
} MapSize;
inline constexpr uint countx = 20u;
inline constexpr uint county = 20u;
inline constexpr float rx = MapSize.x / float(countx);
inline constexpr float ry = MapSize.y / float(county);

class MapChunk {
public:
  MapChunk();
  void init(vec2 bottomLeft);
  Transform transform;
  Renderable renderable;

  ~MapChunk();
  inline uint indexOf(uint x, uint y) const {
    uvec2 coords = clamp<uvec2>({ x,y }, uvec2::zero, { countx - 1, county - 1 });
    return coords.x + coords.y * countx;
  }

  inline float height(uint x, uint y) const { return mHeight[indexOf(x, y)]; }
  inline vec3 normal(uint x, uint y) const { return mNormal[indexOf(x, y)]; }

  inline const aabb3& bound() const { return mBound; }
protected:
  std::array<float, countx * county> mHeight;
  std::array<vec3, countx * county> mNormal;
  aabb3 mBound;
};

class Map {
public:
  void init();
  void addToScene(RenderScene& scene);
  void update();
  inline uint indexOf(uint x, uint y) const {
    uvec2 coords = clamp<uvec2>({ x,y }, uvec2::zero, { countx, county });
    return coords.x + coords.y * countx;
  }

  contact3 raycast(const ray3& ray) const;
  float height(vec2 pos) const;
  vec3 normal(vec2 pos) const;

  inline const aabb3& bound() const { return mBound; };

protected:
  void updateWater();
  aabb3 mBound;
  std::array<MapChunk, countx * county> mChunks;
  Renderable mRenderableWater;
  Transform mTransform;
};
