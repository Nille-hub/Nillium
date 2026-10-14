#include "assets/graphic.h"

#include <cstddef>
#include <span>
#include <stdexcept>

namespace {

constexpr std::size_t headerSize = 8;
constexpr std::size_t columnOffsetSize = 4;
constexpr int endOfColumn = 0xFF;

std::int16_t readInt16(std::span<const std::byte> bytes, std::size_t offset) {
  const auto low = std::to_integer<std::uint16_t>(bytes[offset]);
  const auto high = std::to_integer<std::uint16_t>(bytes[offset + 1]);
  return static_cast<std::int16_t>(static_cast<std::uint16_t>(low | (high << 8)));
}

std::int32_t readInt32(std::span<const std::byte> bytes, std::size_t offset) {
  std::uint32_t value = 0;
  for (std::size_t i = 0; i < 4; ++i) {
    value |= std::to_integer<std::uint32_t>(bytes[offset + i]) << (8 * i);
  }
  return static_cast<std::int32_t>(value);
}

} // namespace

WadGraphic readGraphic(const Wad& wad, std::string_view name) {
  const std::span<const std::byte> bytes = wad.lump(name);
  const std::string label(name);

  if (bytes.size() < headerSize) {
    throw std::runtime_error("graphic " + label + ": too small for a header");
  }

  WadGraphic graphic;
  graphic.name = label;
  graphic.width = readInt16(bytes, 0);
  graphic.height = readInt16(bytes, 2);
  graphic.leftOffset = readInt16(bytes, 4);
  graphic.topOffset = readInt16(bytes, 6);

  if (graphic.width <= 0 || graphic.height <= 0) {
    throw std::runtime_error("graphic " + label + ": invalid size");
  }

  const auto width = static_cast<std::size_t>(graphic.width);
  const auto height = static_cast<std::size_t>(graphic.height);
  if (headerSize + width * columnOffsetSize > bytes.size()) {
    throw std::runtime_error("graphic " + label + ": column offsets lie outside the lump");
  }

  graphic.indices.assign(width * height, 0);
  graphic.opaque.assign(width * height, 0);

  for (std::size_t x = 0; x < width; ++x) {
    const std::int32_t columnStart = readInt32(bytes, headerSize + x * columnOffsetSize);
    if (columnStart < 0) {
      throw std::runtime_error("graphic " + label + ": invalid column offset");
    }

    std::size_t at = static_cast<std::size_t>(columnStart);
    while (true) {
      if (at >= bytes.size()) {
        throw std::runtime_error("graphic " + label + ": column runs past the end of the lump");
      }

      const int topDelta = std::to_integer<int>(bytes[at]);
      if (topDelta == endOfColumn) {
        break;
      }

      // A post: top row, pixel count, one unused byte, the pixels, one unused byte.
      if (at + 2 > bytes.size()) {
        throw std::runtime_error("graphic " + label + ": post header runs past the end of the lump");
      }
      const auto length = std::to_integer<std::size_t>(bytes[at + 1]);
      if (at + 3 + length + 1 > bytes.size()) {
        throw std::runtime_error("graphic " + label + ": post runs past the end of the lump");
      }

      for (std::size_t i = 0; i < length; ++i) {
        const std::size_t y = static_cast<std::size_t>(topDelta) + i;
        if (y >= height) {
          break;
        }
        const std::size_t pixel = y * width + x;
        graphic.indices[pixel] = std::to_integer<std::uint8_t>(bytes[at + 3 + i]);
        graphic.opaque[pixel] = 1;
      }

      at += length + 4;
    }
  }

  return graphic;
}
