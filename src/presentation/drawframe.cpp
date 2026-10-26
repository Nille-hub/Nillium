#include "presentation/drawframe.h"
#include "presentation/mapview.h"
#include "presentation/menuview.h"

#include <glad/glad.h>

void drawFrame(FramebufferSize window, const PatchDrawer& drawer, const Texture& palettes,
               LineDrawer& lines, const GraphicStore& graphics, const Game& game, const Menu& menu) {
  glViewport(0, 0, window.width, window.height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (game.mode() == GameMode::Title) {
    // Pages wider than 320 (widescreen title pictures) extend equally to both sides.
    const GraphicEntry& title = graphics.entry(titlePageName);
    drawer.draw(window, palettes, title, (screenWidth - title.width) / 2, 0);
  } else {
    drawMap(window, lines, palettes, game.map());
  }

  if (menu.isOpen()) {
    drawMenu(window, drawer, palettes, graphics, menu);
  }
}
