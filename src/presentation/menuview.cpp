#include "presentation/menuview.h"

#include <cstddef>

void drawMenu(FramebufferSize window, const PatchDrawer& drawer, const GraphicStore& graphics, const Menu& menu) {
  const MenuDef def = menu.current();

  for (const MenuGraphic& decoration : def.decorations) {
    drawer.draw(window, graphics.entry(decoration.name), decoration.x, decoration.y);
  }

  for (std::size_t i = 0; i < def.items.size(); ++i) {
    drawer.draw(window, graphics.entry(def.items[i].graphic), def.x, def.y + static_cast<int>(i) * menuLineHeight);
  }

  const auto skull = static_cast<std::size_t>(menu.skull());
  drawer.draw(window, graphics.entry(skullGraphics[skull]), def.x + skullOffsetX,
              def.y + skullOffsetY + menu.cursor() * menuLineHeight);
}
