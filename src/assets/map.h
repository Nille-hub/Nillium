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

// A point in map units with a fractional part, as glBSP's split points have.
struct MapPoint {
  float x = 0.0f;
  float y = 0.0f;
};

// A convex piece of one sector, from glBSP's GL nodes. The polygon's corners are in order
// around its edge, so it can be drawn as a triangle fan.
struct MapSubsector {
  std::uint16_t sector = 0;  // index into MapData::sectors
  std::vector<MapPoint> polygon;
};

struct MapSector {
  int floorHeight = 0;
  int ceilingHeight = 0;
  std::string floorFlat;
  std::string ceilingFlat;
  int light = 0;  // 0 (dark) to 255 (bright)
  int special = 0;
  int tag = 0;
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
  int special = 0;
  int tag = 0;
  std::uint16_t front = 0;              // index into MapData::sidedefs
  std::optional<std::uint16_t> back;    // index into MapData::sidedefs; none for a solid wall
};

struct MapThing {
  int x = 0;
  int y = 0;
  int angle = 0;  // degrees, 0 = east, counter-clockwise
  int type = 0;   // 1 = player 1 start
  std::uint16_t flags = 0;  // skill levels and multiplayer, as in vanilla doomdata.h
};

// One side of a BSP node: either another node or a subsector.
struct MapNodeChild {
  bool isSubSector = false;
  std::uint16_t index = 0;  // into MapData::nodes or MapData::subsectors
};

// A BSP node from GL_NODES: a partition line dividing space into a front and a back half.
// A point is in front if it lies to the right of the line, seen from start along direction.
struct MapNode {
  MapPoint partitionStart;
  MapPoint partitionDirection;
  MapNodeChild front;
  MapNodeChild back;
};

struct MapData {
  std::vector<MapVertex> vertexes;
  std::vector<MapSector> sectors;
  std::vector<MapSidedef> sidedefs;
  std::vector<MapLinedef> linedefs;
  std::vector<MapThing> things;
  std::vector<MapSubsector> subsectors;
  std::vector<MapNode> nodes; // the root is the last node
};

// Reads the map whose marker lump is `name`, e.g. E1M1 or MAP01, and its GL nodes after
// the marker GL_<name>. Throws if a marker is missing, the lumps after it are not in the
// expected order or have a broken size, the GL nodes are not glBSP version 2, or any index
// refers to something that does not exist.
MapData readMap(const Wad& wad, std::string_view name);
