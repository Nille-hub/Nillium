#include "app/app.h"
#include "assets/palette.h"
#include "presentation/drawframe.h"

#include <array>
#include <chrono>
#include <string_view>

namespace {

constexpr int ticsPerSecond = 35;

using Clock = std::chrono::steady_clock;

int ticsSince(Clock::time_point start) {
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start);
  return static_cast<int>(elapsed.count() * ticsPerSecond / 1'000'000);
}
} // namespace

App::App(const std::filesystem::path& iwad)
    : m_wad(iwad),
      m_graphics(m_wad, std::array<std::string_view, 1>{titlePageName}),
      m_drawer(readPalettes(m_wad)) {}

void App::run() {
  const Clock::time_point start = Clock::now();
  int lastTic = 0;

  while (!m_video.closeRequested()) {
    m_video.pollEvents();

    const int nowTic = ticsSince(start);
    for (; lastTic < nowTic; ++lastTic) {
      // Simulation step; empty until the game layer exists.
    }

    drawFrame(m_video.framebufferSize(), m_drawer, m_graphics);

    m_video.swapBuffers();
  }
}
