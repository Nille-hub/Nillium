#include "game/menudefs.h"

namespace {

constexpr std::array<MenuGraphic, 1> mainDecorations{{{"M_DOOM", 94, 2}}};

constexpr std::array<std::string_view, 6> doomMainItems{
    "M_NGAME", "M_OPTION", "M_LOADG", "M_SAVEG", "M_RDTHIS", "M_QUITG"};

// Doom II has no "Read This!" item, and its main menu sits 8 units lower.
constexpr std::array<std::string_view, 5> doom2MainItems{
    "M_NGAME", "M_OPTION", "M_LOADG", "M_SAVEG", "M_QUITG"};

constexpr MenuDef doomMain{mainDecorations, doomMainItems, 97, 64};
constexpr MenuDef doom2Main{mainDecorations, doom2MainItems, 97, 72};

} // namespace

const MenuDef& mainMenuDef(GameVariant variant) {
  return variant == GameVariant::Doom2 ? doom2Main : doomMain;
}

std::vector<std::string_view> menuGraphicNames(GameVariant variant) {
  std::vector<std::string_view> names(skullGraphics.begin(), skullGraphics.end());
  const MenuDef& main = mainMenuDef(variant);
  for (const MenuGraphic& decoration : main.decorations) {
    names.push_back(decoration.name);
  }
  names.insert(names.end(), main.items.begin(), main.items.end());
  return names;
}
