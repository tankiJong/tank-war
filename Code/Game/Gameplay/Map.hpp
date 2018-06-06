#pragma once
#include "Engine/Core/common.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/Renderable/Renderable.hpp"
#include "Engine/Math/Transform.hpp"

class Map {
public:
  Renderable renderable;
  void init();
  ~Map();

  uint indexOf(uint x, uint y, uint offsetX, uint offsetY);
  float height(vec2 pos);
protected:
  static constexpr uint countx = 20u;
  static constexpr uint county = 20u;
  static constexpr float rx = 256.f / float(countx);
  static constexpr float ry = 256.f / float(county);

  std::array<float, countx * countx * county * county> mHeight;
  Transform mTransform;
};
