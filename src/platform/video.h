#pragma once

#include "platform/keys.h"

#include <vector>

struct GLFWwindow;

struct FramebufferSize {
  int width = 0;
  int height = 0;
};

class Video {
public:
  Video();
  ~Video();

  Video(const Video&) = delete;
  Video& operator=(const Video&) = delete;

  bool closeRequested() const;
  std::vector<KeyEvent> pollEvents();
  void swapBuffers();
  FramebufferSize framebufferSize() const;

private:
  static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

  GLFWwindow* m_window = nullptr;
  std::vector<KeyEvent> m_keys;
};
