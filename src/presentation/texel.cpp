#include "presentation/texel.h"

// One texel per pixel: red = palette index, green = 255 if opaque, 0 if transparent.
std::vector<std::uint8_t> indexOpaqueTexels(const WadGraphic& graphic) {
  std::vector<std::uint8_t> texels;
  texels.reserve(graphic.indices.size() * 2);
  for (std::size_t i = 0; i < graphic.indices.size(); ++i) {
    texels.push_back(graphic.indices[i]);
    texels.push_back(graphic.opaque[i] != 0 ? 255 : 0);
  }
  return texels;
}
