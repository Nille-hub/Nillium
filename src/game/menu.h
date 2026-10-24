#pragma once

#include "game/game.h"
#include "game/menudefs.h"
#include "game/variant.h"

#include <array>

enum class MenuInput {
  Up,
  Down,
  Select,
  Back,
  Toggle,
  Other,
};

// What the menu decided; App carries it out.
struct MenuAction {
  enum class Kind {
    None,
    NewGame,
  };

  Kind kind = Kind::None;
  int episode = 1;
  Skill skill = Skill::Medium;
};

class Menu {
public:
  Menu(GameVariant variant, int episodes);

  MenuAction respond(MenuInput input, GameMode mode);
  void tic();

  bool isOpen() const;
  MenuDef current() const;
  int cursor() const;
  int skull() const;

private:
  MenuAction select();
  void back();

  GameVariant m_variant;
  int m_episodes;
  MenuId m_current = MenuId::Main;
  // One remembered cursor per menu, as vanilla's lastOn; the skill menu starts at "Hurt Me Plenty".
  std::array<int, 3> m_cursors{0, 0, 2};
  int m_chosenEpisode = 1;
  bool m_open = false;
  int m_skull = 0;
  int m_ticsUntilBlink;
};
