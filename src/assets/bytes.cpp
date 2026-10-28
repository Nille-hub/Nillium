#include "assets/bytes.h"

#include <cctype>

namespace {

constexpr std::size_t nameSize = 8;

} // namespace

std::int16_t readInt16(std::span<const std::byte> bytes, std::size_t offset) {
  const auto low = std::to_integer<std::uint16_t>(bytes[offset]);
  const auto high = std::to_integer<std::uint16_t>(bytes[offset + 1]);
  return static_cast<std::int16_t>(static_cast<std::uint16_t>(low | (high << 8)));
}

std::uint16_t readUInt16(std::span<const std::byte> bytes, std::size_t offset) {
  const auto low = std::to_integer<std::uint16_t>(bytes[offset]);
  const auto high = std::to_integer<std::uint16_t>(bytes[offset + 1]);
  return static_cast<std::uint16_t>(low | (high << 8));
}

std::int32_t readInt32(std::span<const std::byte> bytes, std::size_t offset) {
  std::uint32_t value = 0;

  for (std::size_t i = 0; i < 4; ++i) {
    value |= std::to_integer<std::uint32_t>(bytes[offset + i]) << (8 * i);
  }

  return static_cast<std::int32_t>(value);
}

std::string upperName(std::string_view text) {
  std::string result(text);

  for (char& c : result) {
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  }

  return result;
}

std::string readName(std::span<const std::byte> bytes, std::size_t offset) {
  const auto* first = reinterpret_cast<const char*>(bytes.data() + offset);
  const std::string_view raw(first, nameSize);

  return upperName(raw.substr(0, raw.find('\0')));
}
