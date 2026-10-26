#include "presentation/buffer.h"

#include <utility>

Buffer::Buffer() {
  glCreateBuffers(1, &m_id);
}

Buffer::~Buffer() {
  glDeleteBuffers(1, &m_id);
}

Buffer::Buffer(Buffer&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
  if (this != &other) {
    glDeleteBuffers(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
  }
  return *this;
}

void Buffer::upload(std::span<const std::byte> data) {
  glNamedBufferData(m_id, static_cast<GLsizeiptr>(data.size()), data.data(), GL_DYNAMIC_DRAW);
}

GLuint Buffer::id() const {
  return m_id;
}
