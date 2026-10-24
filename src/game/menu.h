#pragma once

#include "game/menudefs.h"
#include "game/variant.h"

enum class MenuInput {
  Up,
  Down,
  Close,
  Other,
};

class Menu {
public:
  explicit Menu(GameVariant variant);

  void respond(MenuInput input);
  void tic();

  bool isOpen() const;
  const MenuDef& current() const;
  int cursor() const;
  int skull() const;

private:
  const MenuDef* m_current;
  bool m_open = false;
  int m_cursor = 0;
  int m_skull = 0;
  int m_ticsUntilBlink;
};
