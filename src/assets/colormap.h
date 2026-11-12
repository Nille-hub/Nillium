#pragma once

#include "assets/wad.h"

#include <array>
#include <cstddef>
#include <cstdint>

inline constexpr std::size_t colormapRows = 34;
inline constexpr std::size_t colormapLightRows = 32;  // rows 0 (brightest) to 31 (darkest)

// COLORMAP: for each row, the palette index every palette index turns into. Rows 0 to 31 are
// the light levels, row 32 is the invulnerability effect, row 33 is all black.
struct Colormap {
  std::array<std::uint8_t, colormapRows * 256> indices{};  // row r, index i at r * 256 + i
};

Colormap readColormap(const Wad& wad);
