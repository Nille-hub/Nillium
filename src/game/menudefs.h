#pragma once

#include "game/variant.h"

#include <array>
#include <span>
#include <string_view>
#include <vector>

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
  std::span<const std::string_view> items;
  int x = 0;
  int y = 0;
};

inline constexpr int menuLineHeight = 16;
inline constexpr int skullOffsetX = -32;
inline constexpr int skullOffsetY = -5;
inline constexpr std::array<std::string_view, 2> skullGraphics{"M_SKULL1", "M_SKULL2"};

const MenuDef& mainMenuDef(GameVariant variant);

// Every graphic the menus of this variant draw, for the graphic store.
std::vector<std::string_view> menuGraphicNames(GameVariant variant);
