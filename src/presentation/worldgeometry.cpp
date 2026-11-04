#include "presentation/worldgeometry.h"

namespace {

constexpr float maxLight = 255.0f;

MapPoint toPoint(MapVertex vertex) {
  return MapPoint{static_cast<float>(vertex.x), static_cast<float>(vertex.y)};
}

void addWallQuad(std::vector<WorldVertex>& vertices, MapPoint a, MapPoint b, std::uint32_t front,
                 std::uint32_t back, WorldPart bottom, WorldPart top) {
  vertices.push_back({a.x, a.y, front, back, bottom});
  vertices.push_back({b.x, b.y, front, back, bottom});
  vertices.push_back({b.x, b.y, front, back, top});
  vertices.push_back({a.x, a.y, front, back, bottom});
  vertices.push_back({b.x, b.y, front, back, top});
  vertices.push_back({a.x, a.y, front, back, top});
}

// A convex polygon as a fan of triangles around its first corner.
void addFlat(std::vector<WorldVertex>& vertices, const std::vector<MapPoint>& polygon, std::uint32_t sector,
             WorldPart part) {
  for (std::size_t i = 1; i + 1 < polygon.size(); ++i) {
    vertices.push_back({polygon[0].x, polygon[0].y, sector, sector, part});
    vertices.push_back({polygon[i].x, polygon[i].y, sector, sector, part});
    vertices.push_back({polygon[i + 1].x, polygon[i + 1].y, sector, sector, part});
  }
}
} // namespace

std::vector<WorldVertex> buildWorldGeometry(const MapData& map) {
  std::vector<WorldVertex> worldVertexes;

  for (const MapLinedef& line : map.linedefs) {
    const MapPoint start = toPoint(map.vertexes[line.start]);
    const MapPoint end = toPoint(map.vertexes[line.end]);
    const std::uint32_t front = map.sidedefs[line.front].sector;

    if (!line.back) {
      addWallQuad(worldVertexes, start, end,
                  front, front,
                  WorldPart::FrontFloor, WorldPart::FrontCeiling
      );
    } else {
      const std::uint32_t back = map.sidedefs[*line.back].sector;
      addWallQuad(worldVertexes, start, end,
                  front, back,
                  WorldPart::LowerBottom, WorldPart::LowerTop
      );
      addWallQuad(worldVertexes, start, end,
                  front, back,
                  WorldPart::UpperBottom, WorldPart::UpperTop
      );
    }
  }

  for (const MapSubsector& subsector : map.subsectors) {
    addFlat(worldVertexes, subsector.polygon, subsector.sector, WorldPart::FrontFloor);
    addFlat(worldVertexes, subsector.polygon, subsector.sector, WorldPart::FrontCeiling);
  }

  return worldVertexes;
}
