#include "app/app.h"
#include "assets/map.h"
#include "assets/palette.h"
#include "game/game.h"
#include "game/menudefs.h"
#include "game/variant.h"
#include "platform/keys.h"
#include "presentation/drawframe.h"
#include "presentation/palettetexture.h"

#include <chrono>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr int ticsPerSecond = 35;

using Clock = std::chrono::steady_clock;

int ticsSince(Clock::time_point start) {
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start);
  return static_cast<int>(elapsed.count() * ticsPerSecond / 1'000'000);
}

// Doom II's maps are named MAP01 to MAP32, Doom's E1M1 and so on.
GameVariant variantOf(const Wad& wad) {
  return wad.contains("MAP01") ? GameVariant::Doom2 : GameVariant::Doom;
}

// Shareware has episode 1, registered 1 to 3, Ultimate Doom 1 to 4. Doom II has none.
int episodesIn(const Wad& wad) {
  int episodes = 0;
  while (episodes < 4 && wad.contains("E" + std::to_string(episodes + 1) + "M1")) {
    ++episodes;
  }
  return episodes;
}

std::vector<std::string_view> graphicNames(const Wad& wad) {
  std::vector<std::string_view> names = menuGraphicNames(variantOf(wad), episodesIn(wad));
  names.push_back(titlePageName);
  return names;
}

MenuInput toMenuInput(Key key) {
  switch (key) {
  case Key::Up:        return MenuInput::Up;
  case Key::Down:      return MenuInput::Down;
  case Key::Enter:     return MenuInput::Select;
  case Key::Backspace: return MenuInput::Back;
  case Key::Escape:    return MenuInput::Toggle;
  default:             return MenuInput::Other;
  }
}

} // namespace

App::App(const std::filesystem::path& iwad)
    : m_wad(iwad),
      m_menu(variantOf(m_wad), episodesIn(m_wad)),
      m_palettes(makePaletteTexture(readPalettes(m_wad))),
      m_graphics(m_wad, graphicNames(m_wad)) {}

void App::run() {
  const Clock::time_point start = Clock::now();
  int lastTic = 0;
  bool quitChosen = false;

  while (!m_video.closeRequested() && !quitChosen) {
    for (const KeyEvent& event : m_video.pollEvents()) {
      const MenuAction action = m_menu.respond(toMenuInput(event.key), m_game.mode());
      switch (action.kind) {
      case MenuAction::Kind::None:
        break;
      case MenuAction::Kind::NewGame: {
        MapData map = readMap(m_wad, firstMapName(variantOf(m_wad), action.episode));
        m_game.newGame(action.episode, action.skill, std::move(map));
        break;
      }
      case MenuAction::Kind::Quit:
        quitChosen = true;
        break;
      }
    }

    const int nowTic = ticsSince(start);
    for (; lastTic < nowTic; ++lastTic) {
      m_menu.tic();
    }

    drawFrame(m_video.framebufferSize(), m_drawer, m_palettes, m_graphics, m_game, m_menu);

    m_video.swapBuffers();
  }
}
