#pragma once

#include "game/variant.h"

#include <array>
#include <span>
#include <string_view>
#include <vector>

enum class MenuId {
  Main,
  Episode,
  Skill,
};

// What selecting an item does. Episode and skill items take their value from their position.
enum class ItemAction {
  None,
  NewGame,
  ChooseEpisode,
  ChooseSkill,
  Quit,
};

struct MenuItem {
  std::string_view graphic;
  ItemAction action = ItemAction::None;
};

// A graphic drawn at a fixed position whenever its menu is shown, e.g. a title.
struct MenuGraphic {
  std::string_view name;
  int x = 0;
  int y = 0;
};

// One menu, as in vanilla m_menu.c: fixed graphics, then one graphic per item,
// stacked from (x, y) downwards.
struct MenuDef {
  std::span<const MenuGraphic> decorations;
  std::span<const MenuItem> items;
  int x = 0;
  int y = 0;
};

inline constexpr int menuLineHeight = 16;
inline constexpr int skullOffsetX = -32;
inline constexpr int skullOffsetY = -5;
inline constexpr std::array<std::string_view, 2> skullGraphics{"M_SKULL1", "M_SKULL2"};

// The menu as this game shows it; the episode menu lists only the episodes the IWAD contains.
MenuDef menuDef(MenuId id, GameVariant variant, int episodes);

// Every graphic the menus of this game draw, for the graphic store.
std::vector<std::string_view> menuGraphicNames(GameVariant variant, int episodes);
