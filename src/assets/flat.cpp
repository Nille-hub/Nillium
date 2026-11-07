#include "assets/flat.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr int flatSize = 64;

}  // namespace

std::vector<WadGraphic> readFlats(const Wad& wad, std::span<const std::string> names) {
  std::vector<WadGraphic> flats;
  flats.reserve(names.size());

  for (const std::string& name : names) {
    const std::span<const std::byte> lump = wad.lump(name);
    if (lump.size() != static_cast<std::size_t>(flatSize) * flatSize) {
      throw std::runtime_error(std::format(
        "size of lump with name {} is {} but flat lumps should have size {}",
        name, lump.size(), flatSize * flatSize)
      );
    }
    WadGraphic graphic;
    graphic.name = name;
    graphic.width = flatSize;
    graphic.height = flatSize;
    graphic.indices.resize(lump.size());
    std::ranges::transform(lump, graphic.indices.begin(),
                           [](std::byte b) { return std::to_integer<std::uint8_t>(b); }
    );
    graphic.opaque.assign(lump.size(), 1);

    flats.push_back(std::move(graphic));
  }

  return flats;
}
