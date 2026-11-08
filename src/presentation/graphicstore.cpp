#include "presentation/graphicstore.h"
#include "presentation/texel.h"
#include "assets/graphic.h"

#include <stdexcept>

namespace {

GraphicEntry upload(const WadGraphic& graphic) {
  return GraphicEntry{
      Texture(graphic.width, graphic.height, TextureFormat::Rg8, indexOpaqueTexels(graphic)),
      graphic.width,
      graphic.height,
      graphic.leftOffset,
      graphic.topOffset,
  };
}

} // namespace

GraphicStore::GraphicStore(const Wad& wad, std::span<const std::string_view> names) {
  for (const std::string_view name : names) {
    if (!m_entries.contains(name)) {
      m_entries.emplace(std::string(name), upload(readGraphic(wad, name)));
    }
  }
}

const GraphicEntry& GraphicStore::entry(std::string_view name) const {
  const auto found = m_entries.find(name);
  if (found == m_entries.end()) {
    throw std::out_of_range("graphic " + std::string(name) + " is not in the graphic store");
  }
  return found->second;
}
