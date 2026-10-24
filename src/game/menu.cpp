#include "game/menu.h"

#include <cstddef>

namespace {

// Vanilla M_Ticker switches between the two skull graphics every 8 tics.
constexpr int ticsPerBlink = 8;

std::size_t indexOf(MenuId id) {
  return static_cast<std::size_t>(id);
}

} // namespace

Menu::Menu(GameVariant variant, int episodes)
    : m_variant(variant), m_episodes(episodes), m_ticsUntilBlink(ticsPerBlink) {}

MenuAction Menu::respond(MenuInput input, GameMode mode) {
  if (!m_open) {
    // On the title screen any key opens the menu; during a level only the toggle key does.
    if (mode == GameMode::Title || input == MenuInput::Toggle) {
      m_open = true;
      m_current = MenuId::Main;
    }
    return MenuAction{};
  }

  const int itemCount = static_cast<int>(current().items.size());
  int& position = m_cursors[indexOf(m_current)];
  switch (input) {
  case MenuInput::Up:
    position = (position + itemCount - 1) % itemCount;
    break;
  case MenuInput::Down:
    position = (position + 1) % itemCount;
    break;
  case MenuInput::Select:
    return select();
  case MenuInput::Back:
    back();
    break;
  case MenuInput::Toggle:
    m_open = false;
    break;
  case MenuInput::Other:
    break;
  }
  return MenuAction{};
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

MenuDef Menu::current() const {
  return menuDef(m_current, m_variant, m_episodes);
}

int Menu::cursor() const {
  return m_cursors[indexOf(m_current)];
}

int Menu::skull() const {
  return m_skull;
}

MenuAction Menu::select() {
  const int position = cursor();
  switch (current().items[static_cast<std::size_t>(position)].action) {
  case ItemAction::None:
    break;
  case ItemAction::NewGame:
    // Doom II has a single episode, so it goes straight to the skill menu.
    m_chosenEpisode = 1;
    m_current = m_variant == GameVariant::Doom2 ? MenuId::Skill : MenuId::Episode;
    break;
  case ItemAction::ChooseEpisode:
    m_chosenEpisode = position + 1;
    m_current = MenuId::Skill;
    break;
  case ItemAction::ChooseSkill:
    m_open = false;
    return MenuAction{MenuAction::Kind::NewGame, m_chosenEpisode, static_cast<Skill>(position)};
  }
  return MenuAction{};
}

void Menu::back() {
  switch (m_current) {
  case MenuId::Main:
    break;
  case MenuId::Episode:
    m_current = MenuId::Main;
    break;
  case MenuId::Skill:
    m_current = m_variant == GameVariant::Doom2 ? MenuId::Main : MenuId::Episode;
    break;
  }
}
