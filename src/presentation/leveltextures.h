#pragma once

#include "assets/map.h"
#include "assets/wad.h"
#include "presentation/texturearray.h"

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// Every wall texture and flat one map uses, as layers of one texture array, with lookup by
// name. Wall textures and flats are separate namespaces, as in vanilla. Built once per level.
class LevelTextures {
public:
  LevelTextures(const Wad& wad, const MapData& map);

  // The layer holding a wall texture or flat. Throws if the map does not use that name.
  std::uint32_t wallLayer(std::string_view name) const;
  std::uint32_t flatLayer(std::string_view name) const;

  const TextureArray& array() const;

private:
  struct UsedNames {
    std::vector<std::string> walls;
    std::vector<std::string> flats;
  };

  LevelTextures(const Wad& wad, const UsedNames& names);
  static UsedNames usedNames(const MapData& map);

  std::map<std::string, std::uint32_t, std::less<>> m_wallLayers;
  std::map<std::string, std::uint32_t, std::less<>> m_flatLayers;
  TextureArray m_array;
};
