#pragma once

#include "game/game.h"
#include "game/menu.h"
#include "platform/video.h"
#include "presentation/graphicstore.h"
#include "presentation/patchdrawer.h"
#include "presentation/texture.h"

#include <string_view>

inline constexpr std::string_view titlePageName = "TITLEPIC";

void drawFrame(FramebufferSize window, const PatchDrawer& drawer, const Texture& palettes,
               const GraphicStore& graphics, const Game& game, const Menu& menu);
