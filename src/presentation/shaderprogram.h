#pragma once

#include <glad/glad.h>

#include <string_view>

class ShaderProgram {
public:
  ShaderProgram(std::string_view vertexSource, std::string_view fragmentSource);
  ~ShaderProgram();

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&& other) noexcept;
  ShaderProgram& operator=(ShaderProgram&& other) noexcept;

  GLuint id() const;

private:
  GLuint m_id = 0;
};
