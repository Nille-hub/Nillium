#include "presentation/worldgeometry.h"

#include <cmath>
#include <cstdint>
#include <vector>

namespace {

MapPoint toPoint(MapVertex vertex) {
  return MapPoint{static_cast<float>(vertex.x), static_cast<float>(vertex.y)};
}

// What a wall quad needs to know about its linedef, beyond its two corner points.
struct WallSides {
  std::uint32_t frontSector = 0;
  std::uint32_t backSector = 0;
  std::uint32_t frontSide = 0;
  std::uint32_t backSide = 0;
  std::uint32_t flags = 0;
};

void addWallQuad(std::vector<WorldVertex>& vertices, MapPoint a, MapPoint b, const WallSides& sides,
                 WorldPart bottom, WorldPart top) {
  const float length = std::hypot(b.x - a.x, b.y - a.y);
  const auto corner = [&](MapPoint point, float along, WorldPart part) {
    vertices.push_back({point.x, point.y, along, length, sides.frontSector, sides.backSector, sides.frontSide,
                        sides.backSide, sides.flags, part});
  };
  corner(a, 0.0f, bottom);
  corner(b, length, bottom);
  corner(b, length, top);
  corner(a, 0.0f, bottom);
  corner(b, length, top);
  corner(a, 0.0f, top);
}

// A convex polygon as a fan of triangles around its first corner.
void addFlat(std::vector<WorldVertex>& vertices, const std::vector<MapPoint>& polygon, std::uint32_t sector,
             WorldPart part) {
  const auto corner = [&](MapPoint point) {
    WorldVertex vertex;
    vertex.x = point.x;
    vertex.y = point.y;
    vertex.front = sector;
    vertex.back = sector;
    vertex.part = part;
    vertices.push_back(vertex);
  };
  for (std::size_t i = 1; i + 1 < polygon.size(); ++i) {
    corner(polygon[0]);
    corner(polygon[i]);
    corner(polygon[i + 1]);
  }
}

} // namespace

std::vector<WorldVertex> buildWorldGeometry(const MapData& map) {
  std::vector<WorldVertex> worldVertexes;

  for (const MapLinedef& line : map.linedefs) {
    const MapPoint start = toPoint(map.vertexes[line.start]);
    const MapPoint end = toPoint(map.vertexes[line.end]);
    const std::uint32_t frontSector = map.sidedefs[line.front].sector;

    if (!line.back) {
      const WallSides sides{frontSector, frontSector, line.front, line.front, line.flags};
      addWallQuad(worldVertexes, start, end, sides, WorldPart::WallBottom, WorldPart::WallTop);
    } else {
      const WallSides sides{frontSector, map.sidedefs[*line.back].sector, line.front, *line.back, line.flags};
      addWallQuad(worldVertexes, start, end, sides, WorldPart::LowerBottom, WorldPart::LowerTop);
      addWallQuad(worldVertexes, start, end, sides, WorldPart::UpperBottom, WorldPart::UpperTop);
    }
  }

  for (const MapSubsector& subsector : map.subsectors) {
    addFlat(worldVertexes, subsector.polygon, subsector.sector, WorldPart::FrontFloor);
    addFlat(worldVertexes, subsector.polygon, subsector.sector, WorldPart::FrontCeiling);
  }

  return worldVertexes;
}
