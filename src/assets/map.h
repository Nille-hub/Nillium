#pragma once

#include "assets/wad.h"

#include <cstdint>
#include <string_view>
#include <vector>

// A point in map units; x points east, y points north.
struct MapVertex {
  int x = 0;
  int y = 0;
};

struct MapLinedef {
  std::uint16_t start = 0;  // index into MapData::vertexes
  std::uint16_t end = 0;    // index into MapData::vertexes
  bool twoSided = false;    // has a back side: an opening between sectors, not a solid wall
};

struct MapData {
  std::vector<MapVertex> vertexes;
  std::vector<MapLinedef> linedefs;
};

// Reads the map whose marker lump is `name`, e.g. E1M1 or MAP01. Throws if the marker
// is missing, the lumps after it are not in the expected order or have a broken size,
// or a linedef refers to a vertex that does not exist.
MapData readMap(const Wad& wad, std::string_view name);
