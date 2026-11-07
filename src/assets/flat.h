#pragma once

#include "assets/graphic.h"
#include "assets/wad.h"

#include <span>
#include <string>
#include <vector>

// Reads the named flats: 64 x 64 palette indices, row by row, every pixel opaque. The results
// are in the same order as `names`. Throws if a flat is missing or not exactly 4096 bytes.
std::vector<WadGraphic> readFlats(const Wad& wad, std::span<const std::string> names);
