#include "presentation/linedrawer.h"

#include <glad/glad.h>

#include <cstddef>
#include <string_view>

namespace {

constexpr std::string_view vertexSource = R"glsl(
#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in float colorIndex;

// ndc = position * transform.xy + transform.zw
layout(location = 0) uniform vec4 transform;

flat out int paletteIndex;

void main() {
  paletteIndex = int(colorIndex + 0.5);
  gl_Position = vec4(position * transform.xy + transform.zw, 0.0, 1.0);
}
)glsl";

constexpr std::string_view fragmentSource = R"glsl(
#version 460 core

flat in int paletteIndex;
out vec4 color;

layout(binding = 1) uniform sampler2D palettes;

void main() {
  color = vec4(texelFetch(palettes, ivec2(paletteIndex, 0), 0).rgb, 1.0);
}
)glsl";

constexpr GLuint bindingIndex = 0;
constexpr GLuint positionAttribute = 0;
constexpr GLuint colorIndexAttribute = 1;

} // namespace

LineDrawer::LineDrawer() : m_program(vertexSource, fragmentSource) {
  // Describe the vertex layout once: the VAO reads LineVertex records from m_vertices.
  const GLuint vao = m_vertexArray.id();
  glVertexArrayVertexBuffer(vao, bindingIndex, m_vertices.id(), 0, sizeof(LineVertex));

  glEnableVertexArrayAttrib(vao, positionAttribute);
  glVertexArrayAttribFormat(vao, positionAttribute, 2, GL_FLOAT, GL_FALSE, offsetof(LineVertex, x));
  glVertexArrayAttribBinding(vao, positionAttribute, bindingIndex);

  glEnableVertexArrayAttrib(vao, colorIndexAttribute);
  glVertexArrayAttribFormat(vao, colorIndexAttribute, 1, GL_FLOAT, GL_FALSE,
                            offsetof(LineVertex, colorIndex));
  glVertexArrayAttribBinding(vao, colorIndexAttribute, bindingIndex);
}

void LineDrawer::draw(FramebufferSize window, const Texture& palettes, std::span<const LineVertex> vertices,
                      LineTransform transform) {
  m_vertices.upload(std::as_bytes(vertices));
  glViewport(0, 0, window.width, window.height);
  glProgramUniform4f(m_program.id(), 0,
                     transform.scaleX, transform.scaleY,
                     transform.offsetX, transform.offsetY);
  glUseProgram(m_program.id());
  glBindTextureUnit(1, palettes.id());
  glBindVertexArray(m_vertexArray.id());
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
}
