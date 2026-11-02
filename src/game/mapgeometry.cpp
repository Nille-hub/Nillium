#include "game/mapgeometry.h"

#include <cstddef>
#include <vector>

namespace {

bool isBehind(const MapPoint& point, const MapPoint& lineStart, const MapPoint& lineDirection) {
  const float side = (point.x - lineStart.x) * lineDirection.y
                   - (point.y - lineStart.y) * lineDirection.x;
  return side <= 0;
}

std::uint16_t traverseBspForSubsector(const std::vector<MapNode>& nodes,
                                      std::size_t nodeIndex, const MapPoint& point) {
  const MapNode& node = nodes[nodeIndex];
  const bool leftSide = isBehind(point, node.partitionStart, node.partitionDirection);
  const MapNodeChild& child = leftSide ? node.back : node.front;
  if (child.isSubsector) {
    return child.index;
  }
  return traverseBspForSubsector(nodes, child.index, point);
}

} // namespace

std::uint16_t sectorAt(const MapData& map, MapPoint point) {
  if (map.nodes.empty()) {
    return map.subsectors.front().sector;  // a single subsector has no associated nodes tree to walk
  }
  const std::uint16_t subsector = traverseBspForSubsector(map.nodes, map.nodes.size() - 1, point);
  return map.subsectors[subsector].sector;
}
