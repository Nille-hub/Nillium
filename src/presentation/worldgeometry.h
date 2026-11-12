#pragma once

#include "assets/map.h"

#include <cstdint>
#include <vector>

// Which surface a vertex belongs to and which height it takes from the current sector state.
// The shader resolves it each frame, so moving floors and ceilings need no new geometry.
enum class WorldPart : std::uint32_t {
  FrontFloor,    // floor flat: the sector's floor height
  FrontCeiling,  // ceiling flat: the sector's ceiling height
  WallBottom,    // one-sided wall: the front sector's floor
  WallTop,       // one-sided wall: the front sector's ceiling
  LowerBottom,   // two-sided lower step: the lower of both floors
  LowerTop,      // two-sided lower step: the higher of both floors
  UpperBottom,   // two-sided upper step: the lower of both ceilings
  UpperTop,      // two-sided upper step: the higher of both ceilings
  MiddleBottom,  // two-sided middle texture: its bottom, clipped to the opening
  MiddleTop,     // two-sided middle texture: its top, clipped to the opening
};

// One corner of a world triangle. Heights, textures and light come from the sector and sidedef
// state the vertex refers to.
struct WorldVertex {
  float x = 0.0f;
  float y = 0.0f;
  float along = 0.0f;           // walls: distance from the linedef's start vertex
  float lineLength = 0.0f;      // walls: the linedef's length
  float directionX = 0.0f;      // walls: the linedef's x direction from start to end, unit length
  float directionY = 0.0f;      // walls: the linedef's y direction from start to end, unit length
  std::uint32_t front = 0;      // sector index
  std::uint32_t back = 0;       // sector index; equal to front for flats and one-sided walls
  std::uint32_t frontSide = 0;  // walls: sidedef index
  std::uint32_t backSide = 0;   // walls: sidedef index; equal to frontSide for one-sided walls
  std::uint32_t flags = 0;      // walls: the linedef's flags, for pegging
  WorldPart part = WorldPart::FrontFloor;
};

// The level's walls, floors and ceilings as triangles, three vertices each. Every two-sided
// line gets both steps, even where heights are equal now: they may differ later.
std::vector<WorldVertex> buildWorldGeometry(const MapData& map);
