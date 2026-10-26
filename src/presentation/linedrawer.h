#pragma once

#include "platform/video.h"
#include "presentation/buffer.h"
#include "presentation/shaderprogram.h"
#include "presentation/texture.h"
#include "presentation/vertexarray.h"

#include <span>

// One end of a line: a position in the caller's units and a palette index for its colour.
struct LineVertex {
  float x = 0.0f;
  float y = 0.0f;
  float colorIndex = 0.0f;
};

// Maps the caller's units to normalised device coordinates: ndc = position * scale + offset.
struct LineTransform {
  float scaleX = 1.0f;
  float scaleY = 1.0f;
  float offsetX = 0.0f;
  float offsetY = 0.0f;
};

class LineDrawer {
public:
  LineDrawer();

  // Draws one line per pair of vertices. Not const: each call replaces the vertex buffer's contents.
  void draw(FramebufferSize window, const Texture& palettes, std::span<const LineVertex> vertices,
            LineTransform transform);

private:
  ShaderProgram m_program;
  Buffer m_vertices;
  VertexArray m_vertexArray;
};
