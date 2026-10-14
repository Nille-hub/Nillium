#include "app/app.h"
#include "assets/graphic.h"
#include "assets/palette.h"

#include <chrono>

namespace {

constexpr int ticsPerSecond = 35;

using Clock = std::chrono::steady_clock;

int ticsSince(Clock::time_point start) {
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start);
  return static_cast<int>(elapsed.count() * ticsPerSecond / 1'000'000);
}
} // namespace

App::App(const std::filesystem::path& iwad)
    : m_wad(iwad), m_page(readGraphic(m_wad, "TITLEPIC"), readPalettes(m_wad)) {}

void App::run() {
  const Clock::time_point start = Clock::now();
  int lastTic = 0;

  while (!m_video.closeRequested()) {
    m_video.pollEvents();

    const int nowTic = ticsSince(start);
    for (; lastTic < nowTic; ++lastTic) {
      // Simulation step; empty until the game layer exists.
    }

    m_page.draw(m_video.framebufferSize());

    m_video.swapBuffers();
  }
}
