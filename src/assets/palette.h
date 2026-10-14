#pragma once

#include "assets/wad.h"

#include <array>
#include <cstddef>
#include <cstdint>

struct Rgb {
  std::uint8_t r = 0;
  std::uint8_t g = 0;
  std::uint8_t b = 0;
};

struct Palette {
  std::array<Rgb, 256> colors{};
};

inline constexpr std::size_t paletteCount = 14;

std::array<Palette, paletteCount> readPalettes(const Wad& wad);
