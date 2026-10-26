#pragma once

#include "assets/wad.h"
#include "game/game.h"
#include "game/menu.h"
#include "platform/video.h"
#include "presentation/graphicstore.h"
#include "presentation/linedrawer.h"
#include "presentation/patchdrawer.h"
#include "presentation/texture.h"

#include <filesystem>

class App {
public:
  explicit App(const std::filesystem::path& iwad);

  void run();

private:
  Wad m_wad;
  Game m_game;
  Menu m_menu;
  Video m_video;
  Texture m_palettes;
  GraphicStore m_graphics;
  PatchDrawer m_drawer;
  LineDrawer m_lines;
};
