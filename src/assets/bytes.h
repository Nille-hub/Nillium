#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

// Little-endian integers as stored in WAD files, read from `offset` bytes into `bytes`.
// The caller checks that the bytes exist.
std::int16_t readInt16(std::span<const std::byte> bytes, std::size_t offset);
std::int32_t readInt32(std::span<const std::byte> bytes, std::size_t offset);
