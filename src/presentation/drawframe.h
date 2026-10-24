#pragma once

#include "game/menu.h"
#include "platform/video.h"
#include "presentation/graphicstore.h"
#include "presentation/patchdrawer.h"

#include <string_view>

inline constexpr std::string_view titlePageName = "TITLEPIC";

void drawFrame(FramebufferSize window, const PatchDrawer& drawer, const GraphicStore& graphics,
               const Menu& menu);
