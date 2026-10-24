#include "presentation/drawframe.h"
#include "presentation/menuview.h"

#include <glad/glad.h>

void drawFrame(FramebufferSize window, const PatchDrawer& drawer, const GraphicStore& graphics,
               const Menu& menu) {
  glViewport(0, 0, window.width, window.height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Pages wider than 320 (widescreen title pictures) extend equally to both sides.
  const GraphicEntry& title = graphics.entry(titlePageName);
  drawer.draw(window, title, (screenWidth - title.width) / 2, 0);

  if (menu.isOpen()) {
    drawMenu(window, drawer, graphics, menu);
  }
}
