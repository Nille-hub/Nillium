#pragma once

#include "assets/palette.h"
#include "presentation/texture.h"

#include <array>

// One row per palette, 256 colours per row, for colour lookup in shaders.
Texture makePaletteTexture(const std::array<Palette, paletteCount>& palettes);
