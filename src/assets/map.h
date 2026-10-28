#pragma once

#include "assets/wad.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// A point in map units; x points east, y points north.
struct MapVertex {
  int x = 0;
  int y = 0;
};

struct MapSector {
  int floorHeight = 0;
  int ceilingHeight = 0;
  std::string floorFlat;
  std::string ceilingFlat;
  int light = 0;  // 0 (dark) to 255 (bright)
};

// One side of a linedef. Texture names are "-" where vanilla draws nothing.
struct MapSidedef {
  int xOffset = 0;
  int yOffset = 0;
  std::string upperTexture;
  std::string lowerTexture;
  std::string middleTexture;
  std::uint16_t sector = 0;  // index into MapData::sectors
};

// Linedef flags used by the renderer, as in vanilla doomdata.h.
inline constexpr std::uint16_t lineUpperUnpegged = 0x0008;
inline constexpr std::uint16_t lineLowerUnpegged = 0x0010;

struct MapLinedef {
  std::uint16_t start = 0;  // index into MapData::vertexes
  std::uint16_t end = 0;    // index into MapData::vertexes
  std::uint16_t flags = 0;
  std::uint16_t front = 0;              // index into MapData::sidedefs
  std::optional<std::uint16_t> back;    // index into MapData::sidedefs; none for a solid wall
};

struct MapThing {
  int x = 0;
  int y = 0;
  int angle = 0;  // degrees, 0 = east, counter-clockwise
  int type = 0;   // 1 = player 1 start
};

struct MapData {
  std::vector<MapVertex> vertexes;
  std::vector<MapSector> sectors;
  std::vector<MapSidedef> sidedefs;
  std::vector<MapLinedef> linedefs;
  std::vector<MapThing> things;
};

// Reads the map whose marker lump is `name`, e.g. E1M1 or MAP01. Throws if the marker
// is missing, the lumps after it are not in the expected order or have a broken size,
// or any index (vertex, sidedef, sector) refers to something that does not exist.
MapData readMap(const Wad& wad, std::string_view name);
