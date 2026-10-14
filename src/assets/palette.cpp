#include "assets/palette.h"

#include <span>
#include <stdexcept>

namespace {

constexpr std::size_t colorsPerPalette = 256;
constexpr std::size_t bytesPerColor = 3;
constexpr std::size_t bytesPerPalette = colorsPerPalette * bytesPerColor;

} // namespace

std::array<Palette, paletteCount> readPalettes(const Wad& wad) {
  const std::span<const std::byte> bytes = wad.lump("PLAYPAL");
  if (bytes.size() != paletteCount * bytesPerPalette) {
    throw std::runtime_error("PLAYPAL: expected 14 palettes of 256 colours");
  }

  std::array<Palette, paletteCount> palettes{};
  for (std::size_t p = 0; p < paletteCount; ++p) {
    for (std::size_t i = 0; i < colorsPerPalette; ++i) {
      const std::size_t at = p * bytesPerPalette + i * bytesPerColor;
      palettes[p].colors[i] = Rgb{
          std::to_integer<std::uint8_t>(bytes[at]),
          std::to_integer<std::uint8_t>(bytes[at + 1]),
          std::to_integer<std::uint8_t>(bytes[at + 2]),
      };
    }
  }
  return palettes;
}
