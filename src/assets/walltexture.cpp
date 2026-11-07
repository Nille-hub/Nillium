#include "assets/walltexture.h"
#include "assets/bytes.h"

#include <cstdint>
#include <format>
#include <map>
#include <stdexcept>

namespace {

struct PlacedPatch {
  std::string name;
  std::int16_t originX;
  std::int16_t originY;
};

struct Recipe {
  std::int16_t width;
  std::int16_t height;
  std::vector<PlacedPatch> patches;
};

const std::size_t pnamesHeaderByteSize = 4;
const std::size_t texturesHeaderByteSize = 4;
const std::size_t textureRecordHeaderSize = 22;
const std::size_t patchSize = 10;
const std::size_t nameSize = 8;

// in the anonymous namespace
void readRecipes(std::span<const std::byte> lump, std::string_view lumpName,
                 const std::vector<std::string>& patchNames, std::map<std::string, Recipe>& recipes) {
  if (lump.size() < texturesHeaderByteSize) {
    throw std::runtime_error(std::format("{} lump is too small for an expected header of type int32", lumpName));
  }
  std::int32_t textureCount = readInt32(lump, 0);
  if (textureCount < 0 ||
      lump.size() < texturesHeaderByteSize + static_cast<std::size_t>(textureCount) * sizeof(std::int32_t)) {
    throw std::runtime_error(std::format("{} lists {} textures, but has only {} bytes", lumpName, textureCount, lump.size()));
  }

  for (std::size_t i = 0; i < static_cast<std::size_t>(textureCount); ++i) {
    const std::int32_t rawOffset = readInt32(lump, texturesHeaderByteSize + i * sizeof(std::int32_t));
    if (rawOffset < 0 || static_cast<std::size_t>(rawOffset) + textureRecordHeaderSize > lump.size()) {
      throw std::runtime_error(std::format("{}: texture {} has offset {} outside the lump", lumpName, i, rawOffset));
    }
    const auto offset = static_cast<std::size_t>(rawOffset);
    const std::string name = readName(lump, offset);

    Recipe recipe;
    recipe.width = readInt16(lump, offset + 12);
    recipe.height = readInt16(lump, offset + 14);
    const std::int16_t patchCount = readInt16(lump, offset + 20);
    if (recipe.width <= 0 || recipe.height <= 0 || patchCount < 0 ||
        offset + textureRecordHeaderSize + static_cast<std::size_t>(patchCount) * patchSize > lump.size()) {
      throw std::runtime_error(std::format("{}: texture {} has a broken size or patch list", lumpName, name));
    }

    recipe.patches.reserve(static_cast<std::size_t>(patchCount));
    for (std::size_t j = 0; j < static_cast<std::size_t>(patchCount); ++j) {
      const std::size_t at = offset + textureRecordHeaderSize + j * patchSize;
      std::int16_t patchNumber = readInt16(lump, at + 4);

      if (patchNumber < 0 || static_cast<std::size_t>(patchNumber) >= patchNames.size()) {
        throw std::runtime_error(std::format(
          "{}: texture {} refers to patch {}, but PNAMES has {}",
          lumpName, name, patchNumber, patchNames.size()));
      }
      recipe.patches.push_back({patchNames[static_cast<std::size_t>(patchNumber)],
                                readInt16(lump, at), readInt16(lump, at + 2)});
    }
    recipes.try_emplace(name, std::move(recipe));
  }
}
}  // namespace

std::vector<WadGraphic> readWallTextures(const Wad& wad, std::span<const std::string> names) {
  std::span<const std::byte> pnames = wad.lumpAt(wad.index("PNAMES"));
  if (pnames.size() < pnamesHeaderByteSize) {
    throw std::runtime_error(std::format("PNAMES has {} bytes, less than its header", pnames.size()));
  }
  std::int32_t patchCount = readInt32(pnames, 0);
  if (patchCount < 0 || pnames.size() < pnamesHeaderByteSize + static_cast<std::size_t>(patchCount) * nameSize) {
    throw std::runtime_error(std::format(
      "PNAMES lists {} patches, but has only {} bytes", patchCount, pnames.size()));
  }

  std::vector<std::string> patchNames;
  patchNames.reserve(patchCount);

  for (std::size_t i = 0; i < patchCount; ++i) {
    patchNames.push_back(readName(pnames, pnamesHeaderByteSize + i * nameSize));
  }
  if (!wad.contains("TEXTURE1")) {
    throw std::runtime_error("wad file passed on call to readWallTextures is missing TEXTURE1 lump");
  }

  std::map<std::string, Recipe> recipeMap;

  readRecipes(wad.lump("TEXTURE1"), "TEXTURE1", patchNames, recipeMap);

  if (wad.contains("TEXTURE2")) {
    readRecipes(wad.lump("TEXTURE2"), "TEXTURE2", patchNames, recipeMap);
  }

  std::vector<WadGraphic> textures;
  textures.reserve(names.size());

  for (const std::string& name : names) {
    auto rcp = recipeMap.find(name);
    if (rcp == recipeMap.end()) {
      throw std::runtime_error(
        std::format(
          "entry {} from names passed to readWallTexture by caller was not found in TEXTUREx lumps", name));
    }
    WadGraphic texture;
    const Recipe& recipe = rcp->second;
    std::int16_t width = recipe.width;
    std::int16_t height = recipe.height;
    std::size_t canvasSize = static_cast<std::size_t>(width) * height;
    texture.name = rcp->first;
    texture.width = width;
    texture.height = height;
    texture.indices.assign(canvasSize, 0);
    texture.opaque.assign(canvasSize, 0);

    for (const PlacedPatch& placed : recipe.patches) {
      const WadGraphic patch = readGraphic(wad, placed.name);
      for (int py = 0; py < patch.height; ++py) {
        for (int px = 0; px < patch.width; ++px) {
          const auto source = static_cast<std::size_t>(py * patch.width + px);
          const int tx = placed.originX + px;
          const int ty = placed.originY + py;
          if (patch.opaque[source] == 0 || tx < 0 || ty < 0 || tx >= recipe.width || ty >= recipe.height) {
            continue;
          }
          const auto target = static_cast<std::size_t>(ty * recipe.width + tx);
          texture.indices[target] = patch.indices[source];
          texture.opaque[target] = 1;
        }
      }
    }
    textures.push_back(std::move(texture));
  }

  return textures;
}
