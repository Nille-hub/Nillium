#pragma once

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
  void pollEvents();
  void swapBuffers();
  FramebufferSize framebufferSize() const;

private:
  GLFWwindow* m_window = nullptr;
};
