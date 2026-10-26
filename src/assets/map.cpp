#include "assets/map.h"
#include "assets/bytes.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

namespace {

constexpr std::size_t mapLinedefsOffsetFromMarker = 2;
constexpr std::size_t mapVertexesOffsetFromMarker = 4;
constexpr std::size_t mapLinedefRecordSize = 14;
constexpr std::size_t mapVertexRecordSize = 4;
constexpr std::uint16_t noSidedef = 0xFFFF;

std::span<const std::byte> readLump(const Wad& wad, std::size_t markerPos, std::size_t recordSize,
                                    std::size_t offset, std::string_view wantedLumpName) {
  const std::size_t position = markerPos + offset;
  const std::string_view nameAtOffset = wad.nameAt(position);
  if (wantedLumpName != nameAtOffset) {
    throw std::runtime_error(std::format("map {}: expected {} at position {} after the marker, found {}",
                                          wad.nameAt(markerPos), wantedLumpName, offset, nameAtOffset));
  }

  const std::span<const std::byte> lump = wad.lumpAt(position);
  if (lump.size() % recordSize != 0) {
    throw std::runtime_error(std::format("map {}: {} has {} bytes, not a multiple of {}",
                                          wad.nameAt(markerPos), wantedLumpName, lump.size(), recordSize));
  }
  return lump;
}

} // namespace

MapData readMap(const Wad& wad, std::string_view name) {
  MapData mapData;

  const std::size_t marker = wad.index(name);
  const std::span<const std::byte> vertexLump =
    readLump(wad, marker, mapVertexRecordSize, mapVertexesOffsetFromMarker, "VERTEXES");
  const std::size_t vertexCount = vertexLump.size() / mapVertexRecordSize;
  mapData.vertexes.reserve(vertexCount);
  for (std::size_t i = 0; i < vertexCount; ++i) {
    const std::size_t at = i * mapVertexRecordSize;
    const std::int16_t xPos = readInt16(vertexLump, at);
    const std::int16_t yPos = readInt16(vertexLump, at + 2);
    mapData.vertexes.push_back({xPos, yPos});
  }

  const std::span<const std::byte> linedefLump =
    readLump(wad, marker, mapLinedefRecordSize, mapLinedefsOffsetFromMarker, "LINEDEFS");
  const std::size_t linedefCount = linedefLump.size() / mapLinedefRecordSize;
  mapData.linedefs.reserve(linedefCount);
  for (std::size_t i = 0; i < linedefCount; ++i) {
    const std::size_t at = i * mapLinedefRecordSize;
    const std::uint16_t start = readUInt16(linedefLump, at);
    const std::uint16_t end = readUInt16(linedefLump, at + 2);
    const std::uint16_t backSidedef = readUInt16(linedefLump, at + 12);

    if (start >= vertexCount || end >= vertexCount) {
      throw std::runtime_error(std::format("map {}: linedef {} refers to vertex {} or {}, but the map has {} vertexes",
                                           name, i, start, end, vertexCount));
    }
    mapData.linedefs.push_back({start, end, backSidedef != noSidedef});
  }
  return mapData;
}
