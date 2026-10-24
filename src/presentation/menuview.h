#pragma once

#include "game/menu.h"
#include "platform/video.h"
#include "presentation/graphicstore.h"
#include "presentation/patchdrawer.h"

void drawMenu(FramebufferSize window, const PatchDrawer& drawer, const GraphicStore& graphics, const Menu& menu);
