#pragma once

#include "assets/graphic.h"
#include "assets/palette.h"
#include "platform/video.h"
#include "presentation/shaderprogram.h"
#include "presentation/texture.h"
#include "presentation/vertexarray.h"

#include <array>

class Page {
public:
  Page(const WadGraphic& graphic, const std::array<Palette, paletteCount>& palettes);

  void draw(FramebufferSize size) const;

private:
  ShaderProgram m_program;
  VertexArray m_vertexArray;
  Texture m_graphic;
  Texture m_palettes;
};
