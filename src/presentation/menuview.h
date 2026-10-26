#pragma once

#include "game/menu.h"
#include "platform/video.h"
#include "presentation/graphicstore.h"
#include "presentation/patchdrawer.h"
#include "presentation/texture.h"

void drawMenu(FramebufferSize window, const PatchDrawer& drawer, const Texture& palettes,
              const GraphicStore& graphics, const Menu& menu);
