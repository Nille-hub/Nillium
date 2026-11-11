#pragma once

#include <span>
#include <string_view>

// A wall texture or flat that cycles through its frames during play, as in vanilla's animdefs
// (p_spec.c). Frames are listed in display order.
struct AnimationDef {
  bool isWallTexture = false;  // otherwise a flat
  std::span<const std::string_view> frames;
};

// Every animation advances one frame per this many tics, as is the behavior in vanilla.
inline constexpr int ticsPerAnimationFrame = 8;

std::span<const AnimationDef> animationDefs();
