#include "assets/bytes.h"

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
