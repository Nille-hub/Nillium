#pragma once

#include "assets/graphic.h"
#include "assets/wad.h"

#include <span>
#include <string>
#include <vector>

// Composes the named wall textures from their patches, as defined in TEXTURE1 and, if present,
// TEXTURE2. The results are in the same order as `names`. Throws if a name is not defined, or
// a definition refers to a patch that does not exist.
std::vector<WadGraphic> readWallTextures(const Wad& wad, std::span<const std::string> names);
