#include "presentation/palettetexture.h"

namespace {
// One row per palette, 256 RGB texels per row.
std::vector<std::uint8_t> paletteTexels(const std::array<Palette, paletteCount>& palettes) {
  std::vector<std::uint8_t> texels;
  texels.reserve(paletteCount * 256 * 3);
  for (const Palette& palette : palettes) {
    for (const Rgb& color : palette.colors) {
      texels.push_back(color.r);
      texels.push_back(color.g);
      texels.push_back(color.b);
    }
  }
  return texels;
}
} //namespace

Texture makePaletteTexture(const std::array<Palette, paletteCount>& palettes) {
  return Texture(256, static_cast<int>(paletteCount), TextureFormat::Rgb8, paletteTexels(palettes));
}
