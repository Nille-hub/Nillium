#pragma once

#include "assets/wad.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

struct WadGraphic {
  std::string name;
  int width = 0;
  int height = 0;
  int leftOffset = 0;
  int topOffset = 0;
  std::vector<std::uint8_t> indices;
  std::vector<std::uint8_t> opaque;
};

WadGraphic readGraphic(const Wad& wad, std::string_view name);
