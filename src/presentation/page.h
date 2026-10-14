#pragma once

#include "assets/graphic.h"
#include "platform/video.h"
#include "presentation/shaderprogram.h"
#include "presentation/texture.h"
#include "presentation/vertexarray.h"

class Page {
public:
  explicit Page(const WadGraphic& graphic);

  void draw(FramebufferSize size) const;

private:
  ShaderProgram m_program;
  VertexArray m_vertexArray;
  Texture m_graphic;
};
