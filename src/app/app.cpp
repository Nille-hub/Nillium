#include "app/app.h"

#include <glad/glad.h>

#include <chrono>

namespace {

constexpr int ticsPerSecond = 35;

using Clock = std::chrono::steady_clock;

int ticsSince(Clock::time_point start) {
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start);
  return static_cast<int>(elapsed.count() * ticsPerSecond / 1'000'000);
}

} // namespace

App::App(const std::filesystem::path& iwad) : m_wad(iwad) {}

void App::run() {
  const Clock::time_point start = Clock::now();
  int lastTic = 0;

  while (!m_video.closeRequested()) {
    m_video.pollEvents();

    const int nowTic = ticsSince(start);
    for (; lastTic < nowTic; ++lastTic) {
      // Simulation step; empty until the game layer exists.
    }

    const bool evenSecond = (lastTic / ticsPerSecond) % 2 == 0;
    const FramebufferSize size = m_video.framebufferSize();
    glViewport(0, 0, size.width, size.height);
    glClearColor(evenSecond ? 0.25f : 0.10f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_video.swapBuffers();
  }
}
