#pragma once

#include "assets/wad.h"
#include "platform/video.h"
#include "presentation/graphicstore.h"
#include "presentation/patchdrawer.h"

#include <filesystem>

class App {
public:
  explicit App(const std::filesystem::path& iwad);

  void run();

private:
  Wad m_wad;
  Video m_video;
  GraphicStore m_graphics;
  PatchDrawer m_drawer;
};
