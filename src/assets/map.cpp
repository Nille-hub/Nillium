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
constexpr std::size_t mapSectorsOffsetFromMarker = 8;
constexpr std::size_t mapSidedefsOffsetFromMarker = 3;
constexpr std::size_t mapThingsOffsetFromMarker = 1;
constexpr std::size_t mapLinedefRecordSize = 14;
constexpr std::size_t mapVertexRecordSize = 4;
constexpr std::size_t mapSectorRecordSize = 26;
constexpr std::size_t mapSidedefRecordSize = 30;
constexpr std::size_t mapThingRecordSize = 10;
constexpr std::uint16_t noSidedef = 0xFFFF;

std::span<const std::byte> readLump(const Wad& wad, std::size_t markerPos, std::size_t recordSize,
                                    std::size_t offset, std::string_view wantedLumpName) {
  const std::size_t position = markerPos + offset;
  const std::string_view nameAtOffset = wad.nameAt(position);
  if (wantedLumpName != nameAtOffset) {
    throw std::runtime_error(std::format(
      "map {}: expected {} at position {} after the marker, found {}",
      wad.nameAt(markerPos), wantedLumpName, offset, nameAtOffset)
    );
  }

  const std::span<const std::byte> lump = wad.lumpAt(position);
  if (lump.size() % recordSize != 0) {
    throw std::runtime_error(std::format(
      "map {}: {} has {} bytes, not a multiple of {}",
      wad.nameAt(markerPos), wantedLumpName, lump.size(), recordSize)
    );
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

  const std::span<const std::byte> sectorLump =
    readLump(wad, marker, mapSectorRecordSize, mapSectorsOffsetFromMarker, "SECTORS");
  const std::size_t sectorCount = sectorLump.size() / mapSectorRecordSize;
  mapData.sectors.reserve(sectorCount);
  for (std::size_t i = 0; i < sectorCount; ++i) {
    const std::size_t at = i * mapSectorRecordSize;
    const std::int16_t floorHeight = readInt16(sectorLump, at);
    const std::int16_t ceilingHeight = readInt16(sectorLump, at + 2);
    const std::int16_t light = readInt16(sectorLump, at + 20);

    mapData.sectors.push_back({
      floorHeight, ceilingHeight,
      readName(sectorLump, at + 4),
      readName(sectorLump, at + 12),
      light
    });
  }

  const std::span<const std::byte> sidedefLump =
    readLump(wad, marker, mapSidedefRecordSize, mapSidedefsOffsetFromMarker, "SIDEDEFS");
  const std::size_t sidedefCount = sidedefLump.size() / mapSidedefRecordSize;
  mapData.sidedefs.reserve(sidedefCount);
  for (std::size_t i = 0; i < sidedefCount; ++i) {
    const std::size_t at = i * mapSidedefRecordSize;
    const std::int16_t xOffset = readInt16(sidedefLump, at);
    const std::int16_t yOffset = readInt16(sidedefLump, at + 2);
    const std::uint16_t sector = readUInt16(sidedefLump, at + 28);

    if (sector >= sectorCount) {
      throw std::runtime_error(std::format(
        "map {}: sidedef {} refers to sector {}, but the map has {} sectors",
        name, i, sector, sectorCount)
      );
    }

    mapData.sidedefs.push_back({
      xOffset, yOffset,
      readName(sidedefLump, at + 4),
      readName(sidedefLump, at + 12),
      readName(sidedefLump, at + 20),
      sector
    });
  }

  const std::span<const std::byte> linedefLump =
    readLump(wad, marker, mapLinedefRecordSize, mapLinedefsOffsetFromMarker, "LINEDEFS");
  const std::size_t linedefCount = linedefLump.size() / mapLinedefRecordSize;
  mapData.linedefs.reserve(linedefCount);
  for (std::size_t i = 0; i < linedefCount; ++i) {
    const std::size_t at = i * mapLinedefRecordSize;
    const std::uint16_t start = readUInt16(linedefLump, at);
    const std::uint16_t end = readUInt16(linedefLump, at + 2);
    const std::uint16_t flags = readUInt16(linedefLump, at + 4);
    const std::uint16_t front = readUInt16(linedefLump, at + 10);
    const std::uint16_t rawBack = readUInt16(linedefLump, at + 12);
    const std::optional<std::uint16_t> back =
      rawBack != noSidedef ? std::optional{rawBack} : std::nullopt;

    if (start >= vertexCount || end >= vertexCount) {
      throw std::runtime_error(std::format(
        "map {}: linedef {} refers to vertex {} or {}, but the map has {} vertexes",
        name, i, start, end, vertexCount)
      );
    }

    if (front >= sidedefCount) {
      throw std::runtime_error(std::format(
        "map {}: linedef {} refers to front sidedef {}, but the map has {} sidedefs",
        name, i, front, sidedefCount)
      );
    }

    if (back && *back >= sidedefCount) {
      throw std::runtime_error(std::format(
        "map {}: linedef {} refers to back sidedef {}, but the map has {} sidedefs",
        name, i, *back, sidedefCount)
      );
    }
    mapData.linedefs.push_back({start, end, flags, front, back});
  }

  const std::span<const std::byte> thingLump =
    readLump(wad, marker, mapThingRecordSize, mapThingsOffsetFromMarker, "THINGS");
  const std::size_t thingCount = thingLump.size() / mapThingRecordSize;
  mapData.things.reserve(thingCount);
  for (std::size_t i = 0; i < thingCount; ++i) {
    const std::size_t at = i * mapThingRecordSize;
    const std::int16_t x = readInt16(thingLump, at);
    const std::int16_t y = readInt16(thingLump, at + 2);
    const std::int16_t angle = readInt16(thingLump, at + 4);
    const std::int16_t type = readInt16(thingLump, at + 6);

    mapData.things.push_back({x, y, angle, type});
  }
  return mapData;
}
