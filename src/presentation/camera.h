#pragma once

#include "game/game.h"
#include "platform/video.h"

#include <glm/glm.hpp>

// The combined view and projection matrix for looking from `viewpoint` into a window of
// this size. World coordinates are Doom's: x east, y north, z up. The vertical field of view
// is fixed at vanilla's 4:3 value, so wider windows see more to the sides.
glm::mat4 viewProjection(const Viewpoint& viewpoint, FramebufferSize window);
