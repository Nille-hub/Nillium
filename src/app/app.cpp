#include "app/app.h"
#include "assets/palette.h"
#include "game/menudefs.h"
#include "game/variant.h"
#include "platform/keys.h"
#include "presentation/drawframe.h"

#include <chrono>
#include <string_view>
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

std::vector<std::string_view> graphicNames(GameVariant variant) {
  std::vector<std::string_view> names = menuGraphicNames(variant);
  names.push_back(titlePageName);
  return names;
}

MenuInput toMenuInput(Key key) {
  switch (key) {
  case Key::Up:     return MenuInput::Up;
  case Key::Down:   return MenuInput::Down;
  case Key::Escape: return MenuInput::Close;
  default:          return MenuInput::Other;
  }
}

} // namespace

App::App(const std::filesystem::path& iwad)
    : m_wad(iwad),
      m_menu(variantOf(m_wad)),
      m_graphics(m_wad, graphicNames(variantOf(m_wad))),
      m_drawer(readPalettes(m_wad)) {}

void App::run() {
  const Clock::time_point start = Clock::now();
  int lastTic = 0;

  while (!m_video.closeRequested()) {
    for (const KeyEvent& event : m_video.pollEvents()) {
      m_menu.respond(toMenuInput(event.key));
    }

    const int nowTic = ticsSince(start);
    for (; lastTic < nowTic; ++lastTic) {
      m_menu.tic();
    }

    drawFrame(m_video.framebufferSize(), m_drawer, m_graphics, m_menu);

    m_video.swapBuffers();
  }
}
