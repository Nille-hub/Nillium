#include "game/menudefs.h"

#include <cstddef>

namespace {

constexpr std::array<MenuGraphic, 1> mainDecorations{{{"M_DOOM", 94, 2}}};

constexpr std::array<MenuItem, 6> doomMainItems{{
    {"M_NGAME", ItemAction::NewGame},
    {"M_OPTION"},
    {"M_LOADG"},
    {"M_SAVEG"},
    {"M_RDTHIS"},
    {"M_QUITG", ItemAction::Quit},
}};

// Doom II has no "Read This!" item, and its main menu sits 8 units lower.
constexpr std::array<MenuItem, 5> doom2MainItems{{
    {"M_NGAME", ItemAction::NewGame},
    {"M_OPTION"},
    {"M_LOADG"},
    {"M_SAVEG"},
    {"M_QUITG", ItemAction::Quit},
}};

constexpr std::array<MenuGraphic, 1> episodeDecorations{{{"M_EPISOD", 54, 38}}};

constexpr std::array<MenuItem, 4> episodeItems{{
    {"M_EPI1", ItemAction::ChooseEpisode},
    {"M_EPI2", ItemAction::ChooseEpisode},
    {"M_EPI3", ItemAction::ChooseEpisode},
    {"M_EPI4", ItemAction::ChooseEpisode},
}};

constexpr std::array<MenuGraphic, 2> skillDecorations{{{"M_NEWG", 96, 14}, {"M_SKILL", 54, 38}}};

constexpr std::array<MenuItem, 5> skillItems{{
    {"M_JKILL", ItemAction::ChooseSkill},
    {"M_ROUGH", ItemAction::ChooseSkill},
    {"M_HURT",  ItemAction::ChooseSkill},
    {"M_ULTRA", ItemAction::ChooseSkill},
    {"M_NMARE", ItemAction::ChooseSkill},
}};

} // namespace

MenuDef menuDef(MenuId id, GameVariant variant, int episodes) {
  switch (id) {
  case MenuId::Main:
    if (variant == GameVariant::Doom2) {
      return MenuDef{mainDecorations, doom2MainItems, 97, 72};
    }
    return MenuDef{mainDecorations, doomMainItems, 97, 64};
  case MenuId::Episode:
    return MenuDef{episodeDecorations,
                   std::span<const MenuItem>(episodeItems).first(static_cast<std::size_t>(episodes)), 48, 63};
  case MenuId::Skill:
    return MenuDef{skillDecorations, skillItems, 48, 63};
  }
  return MenuDef{};
}

std::vector<std::string_view> menuGraphicNames(GameVariant variant, int episodes) {
  std::vector<std::string_view> names(skullGraphics.begin(), skullGraphics.end());

  std::vector<MenuId> menus{MenuId::Main, MenuId::Skill};
  if (variant == GameVariant::Doom) {
    menus.push_back(MenuId::Episode);
  }

  for (const MenuId id : menus) {
    const MenuDef def = menuDef(id, variant, episodes);
    for (const MenuGraphic& decoration : def.decorations) {
      names.push_back(decoration.name);
    }
    for (const MenuItem& item : def.items) {
      names.push_back(item.graphic);
    }
  }
  return names;
}
