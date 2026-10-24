#pragma once

#include "assets/palette.h"
#include "platform/video.h"
#include "presentation/graphicstore.h"
#include "presentation/shaderprogram.h"
#include "presentation/texture.h"
#include "presentation/vertexarray.h"

#include <array>

// Doom's screen: 320x200 units, shown at 4:3 because its pixels are 1.2 times taller than wide.
inline constexpr int screenWidth = 320;
inline constexpr int screenHeight = 200;

class PatchDrawer {
public:
  explicit PatchDrawer(const std::array<Palette, paletteCount>& palettes);

  // Draws a graphic with its anchor at (x, y) in screen units, honouring its offsets.
  void draw(FramebufferSize window, const GraphicEntry& graphic, int x, int y) const;

private:
  ShaderProgram m_program;
  VertexArray m_vertexArray;
  Texture m_palettes;
};
