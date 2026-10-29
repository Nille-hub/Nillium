#include "assets/map.h"
#include "assets/bytes.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace {

constexpr std::size_t mapLinedefsOffsetFromMarker = 2;
constexpr std::size_t mapVertexesOffsetFromMarker = 4;
constexpr std::size_t mapSectorsOffsetFromMarker = 8;
constexpr std::size_t mapSidedefsOffsetFromMarker = 3;
constexpr std::size_t mapThingsOffsetFromMarker = 1;
constexpr std::size_t mapGlVertexesOffsetFromGlMarker = 1;
constexpr std::size_t mapGlSegmentsOffsetFromGlMarker = 2;
constexpr std::size_t mapGlSubSectorsOffsetFromGlMarker = 3;
constexpr std::size_t mapLinedefRecordSize = 14;
constexpr std::size_t mapVertexRecordSize = 4;
constexpr std::size_t mapSectorRecordSize = 26;
constexpr std::size_t mapSidedefRecordSize = 30;
constexpr std::size_t mapThingRecordSize = 10;
constexpr std::size_t mapGlVertexHeaderSize = 4;
constexpr std::size_t mapGlVertexRecordSize = 8;
constexpr std::size_t mapGlSegmentRecordSize = 10;
constexpr std::size_t mapGlSubSectorRecordSize = 4;
constexpr std::uint16_t noSidedef = 0xFFFF;
constexpr std::uint16_t noLinedef = 0xFFFF;
constexpr std::uint16_t glVertexFlag = 0x8000;
constexpr std::uint16_t glVertexMask = 0x7FFF;
constexpr float fixedPointDivisor1616 = 65536.0f;

struct RawGlSeg {
  std::uint16_t startValue;
  std::uint16_t endValue;
  MapPoint start;
  std::optional<std::uint16_t> linedef;
  std::uint16_t side;
};

std::span<const std::byte> readLump(const Wad& wad, std::size_t markerPos, std::size_t recordSize,
                                    std::size_t offset, std::string_view wantedLumpName, std::size_t headerSize) {
  const std::size_t position = markerPos + offset;
  const std::string_view nameAtOffset = wad.nameAt(position);
  if (wantedLumpName != nameAtOffset) {
    throw std::runtime_error(std::format(
      "map {}: expected {} at position {} after the marker, found {}",
      wad.nameAt(markerPos), wantedLumpName, offset, nameAtOffset)
    );
  }

  const std::span<const std::byte> lump = wad.lumpAt(position);
  if (lump.size() < headerSize) {
    throw std::runtime_error(std::format(
      "map {}: {} has {} bytes, less than its {}-byte header",
      wad.nameAt(markerPos), wantedLumpName, lump.size(), headerSize)
    );
  }
  if ((lump.size() - headerSize) % recordSize != 0) {
    throw std::runtime_error(std::format(
      "map {}: {} has {} bytes after its {}-byte header, not a multiple of {}",
      wad.nameAt(markerPos), wantedLumpName, lump.size() - headerSize, headerSize, recordSize)
    );
  }
  return lump;
}

} // namespace

MapData readMap(const Wad& wad, std::string_view name) {
  MapData mapData;
  const std::size_t marker = wad.index(name);

  const std::span<const std::byte> vertexLump =
    readLump(wad, marker, mapVertexRecordSize, mapVertexesOffsetFromMarker, "VERTEXES", 0);
  const std::size_t vertexCount = vertexLump.size() / mapVertexRecordSize;
  mapData.vertexes.reserve(vertexCount);
  for (std::size_t i = 0; i < vertexCount; ++i) {
    const std::size_t at = i * mapVertexRecordSize;
    const std::int16_t xPos = readInt16(vertexLump, at);
    const std::int16_t yPos = readInt16(vertexLump, at + 2);
    mapData.vertexes.push_back({xPos, yPos});
  }

  const std::span<const std::byte> sectorLump =
    readLump(wad, marker, mapSectorRecordSize, mapSectorsOffsetFromMarker, "SECTORS", 0);
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
    readLump(wad, marker, mapSidedefRecordSize, mapSidedefsOffsetFromMarker, "SIDEDEFS", 0);
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
    readLump(wad, marker, mapLinedefRecordSize, mapLinedefsOffsetFromMarker, "LINEDEFS", 0);
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
    readLump(wad, marker, mapThingRecordSize, mapThingsOffsetFromMarker, "THINGS", 0);
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

  const std::string glMarkerName = "GL_" + std::string(name);
  if (!wad.contains(glMarkerName)) {
    throw std::runtime_error(std::format(
      "map {} has no GL nodes ({} not found); run the glBSP node builder on the WAD", name, glMarkerName));
  }
  const std::size_t glMarker = wad.index(glMarkerName);

  const std::span<const std::byte> glVertexLump =
    readLump(wad, glMarker, mapGlVertexRecordSize, mapGlVertexesOffsetFromGlMarker,
            "GL_VERT", mapGlVertexHeaderSize);
  const std::string_view glVertexMagic(
    reinterpret_cast<const char*>(glVertexLump.data()), mapGlVertexHeaderSize
  );
  if (glVertexMagic != "gNd2") {
    throw std::runtime_error(std::format(
      "map {}: GL_VERT has format {}, expected gNd2 (glBSP version 2)",
      name, glVertexMagic));
  }
  const std::span<const std::byte> glVertexData = glVertexLump.subspan(mapGlVertexHeaderSize);
  const std::size_t glVertexCount = glVertexData.size() / mapGlVertexRecordSize;
  std::vector<MapPoint> glVertexes;
  glVertexes.reserve(glVertexCount);

  for (std::size_t i = 0; i < glVertexCount; ++i) {
    const std::size_t at = i * mapGlVertexRecordSize;
    const std::int32_t x = readInt32(glVertexData, at);
    const std::int32_t y = readInt32(glVertexData, at + 4);

    glVertexes.push_back({
      static_cast<float>(x) / fixedPointDivisor1616,
      static_cast<float>(y) / fixedPointDivisor1616
    });
  }

  // A GL seg vertex number refers to GL_VERT if glVertexFlag is set, otherwise to VERTEXES.
  const auto resolveSegVertex = [&](std::uint16_t value, std::size_t seg) -> MapPoint {
    const std::uint16_t index = value & glVertexMask;
    if ((value & glVertexFlag) != 0) {
      if (index >= glVertexCount) {
        throw std::runtime_error(std::format(
          "map {}: GL seg {} refers to GL vertex {}, but the map has {} GL vertexes",
          name, seg, index, glVertexCount));
      }
      return glVertexes[index];
    }
    if (index >= vertexCount) {
      throw std::runtime_error(std::format(
        "map {}: GL seg {} refers to vertex {}, but the map has {} vertexes", name, seg, index, vertexCount));
    }
    const MapVertex& vertex = mapData.vertexes[index];
    return MapPoint{static_cast<float>(vertex.x), static_cast<float>(vertex.y)};
  };

  const std::span<const std::byte> glSegmentLump =
    readLump(wad, glMarker, mapGlSegmentRecordSize, mapGlSegmentsOffsetFromGlMarker, "GL_SEGS", 0);
  const std::size_t glSegmentCount = glSegmentLump.size() / mapGlSegmentRecordSize;
  std::vector<RawGlSeg> rawGlSegments;
  rawGlSegments.reserve(glSegmentCount);

  for (std::size_t i = 0; i < glSegmentCount; ++i) {
    const std::size_t at = i * mapGlSegmentRecordSize;
    const std::uint16_t startValue = readUInt16(glSegmentLump, at);
    const std::uint16_t endValue = readUInt16(glSegmentLump, at + 2);
    const std::uint16_t linedefValue = readUInt16(glSegmentLump, at + 4);
    const std::uint16_t side = readUInt16(glSegmentLump, at + 6);

    const MapPoint start = resolveSegVertex(startValue, i);
    resolveSegVertex(endValue, i);  // only validates: the end point is the next seg's start

    const std::optional<std::uint16_t> linedef =
      linedefValue != noLinedef ? std::optional{linedefValue} : std::nullopt;

    if (linedef && *linedef >= linedefCount) {
      throw std::runtime_error(std::format(
        "map {}: GL seg {} refers to linedef {}, but the map has {} linedefs", name, i, *linedef, linedefCount));
    }
    if (linedef && (side > 1 || (side == 1 && !mapData.linedefs[*linedef].back))) {
      throw std::runtime_error(std::format(
        "map {}: GL seg {} lies on side {} of linedef {}, which has no such side", name, i, side, *linedef));
    }

    rawGlSegments.push_back({startValue, endValue, start, linedef, side});
  }

  const std::span<const std::byte> glSubSectorLump =
    readLump(wad, glMarker, mapGlSubSectorRecordSize, mapGlSubSectorsOffsetFromGlMarker, "GL_SSECT", 0);
  const std::size_t glSubSectorCount = glSubSectorLump.size() / mapGlSubSectorRecordSize;
  mapData.subsectors.reserve(glSubSectorCount);

  for (std::size_t i = 0; i < glSubSectorCount; ++i) {
    const std::size_t at = i * mapGlSubSectorRecordSize;
    const std::size_t count = readUInt16(glSubSectorLump, at);
    const std::size_t first = readUInt16(glSubSectorLump, at + 2);
    const std::size_t pastLast = first + count;

    if (count == 0 || pastLast > glSegmentCount) {
      throw std::runtime_error(std::format(
        "map {}: GL subsector {} uses segs {} to {}, but the map has {} GL segs",
        name, i, first, pastLast, glSegmentCount));
    }

    std::vector<MapPoint> polygon;
    polygon.reserve(count);
    std::optional<std::size_t> sectorSeg;

    for (std::size_t s = first; s < pastLast; ++s) {
      const RawGlSeg& seg = rawGlSegments[s];
      const std::size_t next = (s + 1 == pastLast) ? first : s + 1;
      if (seg.endValue != rawGlSegments[next].startValue) {
        throw std::runtime_error(std::format(
          "map {}: GL subsector {} is not closed: seg {} does not end where seg {} starts", name, i, s, next));
      }

      polygon.push_back(seg.start);
      if (!sectorSeg && seg.linedef) {
        sectorSeg = s;
      }
    }

    if (!sectorSeg) {
      throw std::runtime_error(std::format("map {}: GL subsector {} consists only of minisegs", name, i));
    }

    const RawGlSeg& seg = rawGlSegments[*sectorSeg];
    const MapLinedef& line = mapData.linedefs[*seg.linedef];
    const MapSidedef& side = mapData.sidedefs[seg.side == 0 ? line.front : *line.back];
    mapData.subsectors.push_back({side.sector, std::move(polygon)});
  }

  return mapData;
}
