#pragma once

#include "assets/map.h"
#include "game/game.h"
#include "platform/video.h"
#include "presentation/linedrawer.h"
#include "presentation/texture.h"

// Draws the map's linedefs from above, fitted into the window: walls red, openings grey,
// and the viewpoint as a yellow line pointing where it looks.
void drawMap(FramebufferSize window, LineDrawer& lines, const Texture& palettes, const MapData& map,
             const Viewpoint& viewpoint);
