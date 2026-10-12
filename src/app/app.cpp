#include <app/app.h>

#include <glad/glad.h>

void App::run() {
  while (!m_video.closeRequested()) {
    m_video.pollEvents();

    const FramebufferSize size = m_video.framebufferSize();
    glViewport(0, 0, size.width, size.height);
    glClearColor(0.25f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_video.swapBuffers();
  }
}