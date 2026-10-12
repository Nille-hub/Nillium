#include "platform/video.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

Video::Video() {
  if (!glfwInit()) {
    throw std::runtime_error("glfwInit failed");
  }

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
  if (!mode) {
    glfwTerminate();
    throw std::runtime_error("no primary monitor");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window = glfwCreateWindow(mode->width / 2, mode->height / 2,
                              "Nillium", nullptr, nullptr);

  if (!m_window) {
    glfwTerminate();
    throw std::runtime_error("glfwCreateWindow failed");
  }

  glfwMakeContextCurrent(m_window);
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwDestroyWindow(m_window);
    glfwTerminate();
    throw std::runtime_error("gladLoadGLLoader failed");
  }

  glfwSwapInterval(1);
}

Video::~Video() {
  glfwDestroyWindow(m_window);
  glfwTerminate();
}

bool Video::closeRequested() const {
  return glfwWindowShouldClose(m_window) == GLFW_TRUE;
}

void Video::pollEvents() {
  glfwPollEvents();
}

void Video::swapBuffers() {
  glfwSwapBuffers(m_window);
}

FramebufferSize Video::framebufferSize() const {
  FramebufferSize size;
  glfwGetFramebufferSize(m_window, &size.width, &size.height);
  return size;
}
