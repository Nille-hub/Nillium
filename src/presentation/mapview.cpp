#include "presentation/mapview.h"

#include <algorithm>
#include <vector>

namespace {

constexpr float margin = 0.95f;
constexpr float wallColor = 176.0f;
constexpr float openingColor = 96.0f;

} // namespace

void drawMap(FramebufferSize window, LineDrawer& lines, const Texture& palettes, const MapData& map) {
  if (map.vertexes.empty()) {
    return;
  }

  if (window.width <= 0 || window.height <= 0) {
    return;
  }

  int minX = map.vertexes.front().x;
  int maxX = minX;
  int minY = map.vertexes.front().y;
  int maxY = minY;

  for (const MapVertex& vx : map.vertexes) {
    minX = std::min(minX, vx.x);
    maxX = std::max(maxX, vx.x);
    minY = std::min(minY, vx.y);
    maxY = std::max(maxY, vx.y);
  }

  const float width = std::max(static_cast<float>(maxX - minX), 1.0f);
  const float height = std::max(static_cast<float>(maxY - minY), 1.0f);
  const float aspect = static_cast<float>(window.width) / static_cast<float>(window.height);
  const float scale = 2 * margin * std::min(aspect / width, 1 / height);
  const float scaleX = scale / aspect;
  const float scaleY = scale;
  const LineTransform transform{
    scaleX,
    scaleY,
    -(static_cast<float>(minX) + width / 2) * scaleX,
    -(static_cast<float>(minY) + height / 2) * scaleY
  };

  std::vector<LineVertex> lineVertices;
  lineVertices.reserve(map.linedefs.size() * 2);
  for (const MapLinedef& line : map.linedefs) {
    const float lineColor = line.twoSided ? openingColor : wallColor;
    const MapVertex start = map.vertexes[line.start];
    lineVertices.push_back({static_cast<float>(start.x), static_cast<float>(start.y), lineColor});
    const MapVertex end = map.vertexes[line.end];
    lineVertices.push_back({static_cast<float>(end.x), static_cast<float>(end.y), lineColor});
  }

  lines.draw(window, palettes, lineVertices, transform);
}
