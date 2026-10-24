#include "game/menu.h"

namespace {

// Vanilla M_Ticker switches between the two skull graphics every 8 tics.
constexpr int ticsPerBlink = 8;

} // namespace

Menu::Menu(GameVariant variant) : m_current(&mainMenuDef(variant)), m_ticsUntilBlink(ticsPerBlink) {}

void Menu::respond(MenuInput input) {
  if (!m_open) {
    // On the title screen, any key opens the menu.
    m_open = true;
    return;
  }

  const int itemCount = static_cast<int>(m_current->items.size());
  switch (input) {
  case MenuInput::Up:
    m_cursor = (m_cursor + itemCount - 1) % itemCount;
    break;
  case MenuInput::Down:
    m_cursor = (m_cursor + 1) % itemCount;
    break;
  case MenuInput::Close:
    m_open = false;
    break;
  case MenuInput::Other:
    break;
  }
}

void Menu::tic() {
  if (--m_ticsUntilBlink <= 0) {
    m_skull = 1 - m_skull;
    m_ticsUntilBlink = ticsPerBlink;
  }
}

bool Menu::isOpen() const {
  return m_open;
}

const MenuDef& Menu::current() const {
  return *m_current;
}

int Menu::cursor() const {
  return m_cursor;
}

int Menu::skull() const {
  return m_skull;
}
