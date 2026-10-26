#pragma once

#include <glad/glad.h>

#include <cstddef>
#include <span>

// A GL buffer object. Its contents can be replaced; the buffer itself stays the same object.
class Buffer {
public:
  Buffer();
  ~Buffer();

  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  Buffer(Buffer&& other) noexcept;
  Buffer& operator=(Buffer&& other) noexcept;

  // Replaces the whole contents with `data`.
  void upload(std::span<const std::byte> data);

  GLuint id() const;

private:
  GLuint m_id = 0;
};
