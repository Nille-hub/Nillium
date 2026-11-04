#pragma once

#include "assets/map.h"

#include <cstdint>
#include <vector>

// Which height a vertex takes from the current sector state. The shader resolves it each
// frame, so moving floors and ceilings need no new geometry.
enum class WorldPart : std::uint32_t {
  FrontFloor,    // the front sector's floor: floors, bottoms of one-sided walls
  FrontCeiling,  // the front sector's ceiling: ceilings, tops of one-sided walls
  LowerBottom,   // two-sided lower step: the lower of both floors
  LowerTop,      // two-sided lower step: the higher of both floors
  UpperBottom,   // two-sided upper step: the lower of both ceilings
  UpperTop,      // two-sided upper step: the higher of both ceilings
};

// One corner of a world triangle. Its height and light come from the sectors it refers to.
struct WorldVertex {
  float x = 0.0f;
  float y = 0.0f;
  std::uint32_t front = 0;  // sector index
  std::uint32_t back = 0;   // sector index; equal to front for flats and one-sided walls
  WorldPart part = WorldPart::FrontFloor;
};

// The level's walls, floors and ceilings as triangles, three vertices each. Every two-sided
// line gets both steps, even where heights are equal now: they may differ later.
std::vector<WorldVertex> buildWorldGeometry(const MapData& map);
