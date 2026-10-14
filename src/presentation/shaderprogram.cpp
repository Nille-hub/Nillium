#include "presentation/shaderprogram.h"

#include <stdexcept>
#include <string>
#include <utility>

namespace {

GLuint compile(GLenum type, std::string_view source) {
  const GLuint shader = glCreateShader(type);
  const GLchar* text = source.data();
  const GLint length = static_cast<GLint>(source.size());
  glShaderSource(shader, 1, &text, &length);
  glCompileShader(shader);

  GLint compiled = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled != GL_TRUE) {
    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength), '\0');
    GLsizei written = 0;
    glGetShaderInfoLog(shader, logLength, &written, log.data());
    log.resize(static_cast<std::size_t>(written));
    glDeleteShader(shader);
    const char* stage = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    throw std::runtime_error(std::string(stage) + " shader failed to compile: " + log);
  }
  return shader;
}

} // namespace

ShaderProgram::ShaderProgram(std::string_view vertexSource, std::string_view fragmentSource) {
  const GLuint vertex = compile(GL_VERTEX_SHADER, vertexSource);
  GLuint fragment = 0;
  try {
    fragment = compile(GL_FRAGMENT_SHADER, fragmentSource);
  } catch (...) {
    glDeleteShader(vertex);
    throw;
  }

  m_id = glCreateProgram();
  glAttachShader(m_id, vertex);
  glAttachShader(m_id, fragment);
  glLinkProgram(m_id);
  glDetachShader(m_id, vertex);
  glDetachShader(m_id, fragment);
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  GLint linked = GL_FALSE;
  glGetProgramiv(m_id, GL_LINK_STATUS, &linked);
  if (linked != GL_TRUE) {
    GLint logLength = 0;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength), '\0');
    GLsizei written = 0;
    glGetProgramInfoLog(m_id, logLength, &written, log.data());
    log.resize(static_cast<std::size_t>(written));
    glDeleteProgram(m_id);
    throw std::runtime_error("shader program failed to link: " + log);
  }
}

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(m_id);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
  if (this != &other) {
    glDeleteProgram(m_id);
    m_id = std::exchange(other.m_id, 0);
  }
  return *this;
}

GLuint ShaderProgram::id() const {
  return m_id;
}
