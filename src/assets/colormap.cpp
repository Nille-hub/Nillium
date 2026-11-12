#include "assets/colormap.h"

#include <algorithm>
#include <format>
#include <span>
#include <stdexcept>

Colormap readColormap(const Wad& wad) {
  std::span<const std::byte> lump = wad.lump("COLORMAP");
  const std::size_t size = colormapRows * 256;
  if (lump.size_bytes() < size) {
    throw std::out_of_range(
      std::format("COLORMAP size is smaller than expected - Expected: {} bytes", size)
    );
  }
  Colormap colorMap;
  std::ranges::transform(lump.first(size), colorMap.indices.begin(),
                          [](std::byte b) { return std::to_integer<std::uint8_t>(b); }
  );

  return colorMap;
}
