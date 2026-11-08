#include "presentation/leveltextures.h"
#include "assets/flat.h"
#include "assets/walltexture.h"

#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::string_view noTexture = "-";

std::map<std::string, std::uint32_t, std::less<>> layerIndex(const std::vector<std::string>& names,
                                                             std::uint32_t firstLayer) {
  std::map<std::string, std::uint32_t, std::less<>> layers;
  for (std::size_t i = 0; i < names.size(); ++i) {
    layers.emplace(names[i], firstLayer + static_cast<std::uint32_t>(i));
  }
  return layers;
}

std::vector<WadGraphic> allGraphics(const Wad& wad, const std::vector<std::string>& walls,
                                    const std::vector<std::string>& flats) {
  std::vector<WadGraphic> graphics = readWallTextures(wad, walls);
  graphics.append_range(readFlats(wad, flats));

  return graphics;
}

std::uint32_t layerOf(const std::map<std::string, std::uint32_t, std::less<>>& layers, std::string_view name,
                      std::string_view kind) {
  const auto found = layers.find(name);
  if (found == layers.end()) {
    throw std::out_of_range(std::format("{} {} is not used by this map", kind, name));
  }
  return found->second;
}

} // namespace

LevelTextures::LevelTextures(const Wad& wad, const MapData& map) : LevelTextures(wad, usedNames(map)) {}

LevelTextures::LevelTextures(const Wad& wad, const UsedNames& names)
  : m_wallLayers(layerIndex(names.walls, 0)),
    m_flatLayers(layerIndex(names.flats, static_cast<std::uint32_t>(names.walls.size()))),
    m_array(allGraphics(wad, names.walls, names.flats)) {}

std::uint32_t LevelTextures::wallLayer(std::string_view name) const {
  return layerOf(m_wallLayers, name, "wall texture");
}

std::uint32_t LevelTextures::flatLayer(std::string_view name) const {
  return layerOf(m_flatLayers, name, "flat");
}

const TextureArray& LevelTextures::array() const {
  return m_array;
}

LevelTextures::UsedNames LevelTextures::usedNames(const MapData& map) {
  std::set<std::string> walls;

  const auto addWall = [&](const std::string& name) {
    if (name != noTexture) {
      walls.insert(name);
    }
  };

  for (const MapSidedef& sidedef : map.sidedefs) {
    addWall(sidedef.upperTexture);
    addWall(sidedef.middleTexture);
    addWall(sidedef.lowerTexture);
  }

  std::set<std::string> flats;

  for (const MapSector& sector : map.sectors) {
    flats.insert(sector.floorFlat);
    flats.insert(sector.ceilingFlat);
  }

  return { std::vector<std::string>(walls.begin(), walls.end()),
           std::vector<std::string>(flats.begin(), flats.end()) };
}
