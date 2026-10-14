#include "presentation/vertexarray.h"

#include <utility>

VertexArray::VertexArray() {
  glCreateVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &m_id);
}

VertexArray::VertexArray(VertexArray&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
  if (this != &other) {
    glDeleteVertexArrays(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
  }
  return *this;
}

GLuint VertexArray::id() const {
  return m_id;
}
