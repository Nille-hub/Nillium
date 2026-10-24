#include "presentation/patchdrawer.h"

#include <glad/glad.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace {

constexpr std::string_view vertexSource = R"glsl(
#version 460 core

// The graphic's rectangle in normalised device coordinates: left, top, right, bottom.
layout(location = 0) uniform vec4 rect;

out vec2 uv;

void main() {
  // Four corners drawn as a triangle strip: (0,0) (1,0) (0,1) (1,1), y pointing up.
  vec2 corner = vec2(gl_VertexID & 1, gl_VertexID >> 1);
  // Texture row 0 is the top row of the graphic.
  uv = vec2(corner.x, 1.0 - corner.y);
  gl_Position = vec4(mix(rect.x, rect.z, corner.x), mix(rect.w, rect.y, corner.y), 0.0, 1.0);
}
)glsl";

constexpr std::string_view fragmentSource = R"glsl(
#version 460 core

in vec2 uv;
out vec4 color;

layout(binding = 0) uniform sampler2D graphic;
layout(binding = 1) uniform sampler2D palettes;

void main() {
  vec2 texel = texture(graphic, uv).rg;
  if (texel.g < 0.5) {
    discard;
  }
  int index = int(texel.r * 255.0 + 0.5);
  color = vec4(texelFetch(palettes, ivec2(index, 0), 0).rgb, 1.0);
}
)glsl";

// One row per palette, 256 RGB texels per row.
std::vector<std::uint8_t> paletteTexels(const std::array<Palette, paletteCount>& palettes) {
  std::vector<std::uint8_t> texels;
  texels.reserve(paletteCount * 256 * 3);
  for (const Palette& palette : palettes) {
    for (const Rgb& color : palette.colors) {
      texels.push_back(color.r);
      texels.push_back(color.g);
      texels.push_back(color.b);
    }
  }
  return texels;
}

Texture makePaletteTexture(const std::array<Palette, paletteCount>& palettes) {
  return Texture(256, static_cast<int>(paletteCount), TextureFormat::Rgb8, paletteTexels(palettes));
}

} // namespace

PatchDrawer::PatchDrawer(const std::array<Palette, paletteCount>& palettes)
    : m_program(vertexSource, fragmentSource), m_palettes(makePaletteTexture(palettes)) {}

void PatchDrawer::draw(FramebufferSize window, const GraphicEntry& graphic, int x, int y) const {
  const auto windowWidth = static_cast<float>(window.width);
  const auto windowHeight = static_cast<float>(window.height);
  if (windowWidth <= 0.0f || windowHeight <= 0.0f) {
    return;
  }

  // The 4:3 area that shows Doom's 320x200 screen, centred in the window.
  float areaWidth = windowWidth;
  float areaHeight = windowHeight;
  if (windowWidth * 3.0f > windowHeight * 4.0f) {
    areaWidth = windowHeight * 4.0f / 3.0f;
  } else {
    areaHeight = windowWidth * 3.0f / 4.0f;
  }
  const float areaLeft = (windowWidth - areaWidth) / 2.0f;
  const float areaTop = (windowHeight - areaHeight) / 2.0f;
  const float unitX = areaWidth / static_cast<float>(screenWidth);
  const float unitY = areaHeight / static_cast<float>(screenHeight);

  // The graphic's rectangle in window pixels, measured from the top-left corner.
  const float left = areaLeft + static_cast<float>(x - graphic.leftOffset) * unitX;
  const float top = areaTop + static_cast<float>(y - graphic.topOffset) * unitY;
  const float right = left + static_cast<float>(graphic.width) * unitX;
  const float bottom = top + static_cast<float>(graphic.height) * unitY;

  // Window pixels to normalised device coordinates, where y points up.
  const auto toNdcX = [&](float px) { return px / windowWidth * 2.0f - 1.0f; };
  const auto toNdcY = [&](float py) { return 1.0f - py / windowHeight * 2.0f; };

  glViewport(0, 0, window.width, window.height);
  glProgramUniform4f(m_program.id(), 0, toNdcX(left), toNdcY(top), toNdcX(right), toNdcY(bottom));
  glUseProgram(m_program.id());
  glBindTextureUnit(0, graphic.texture.id());
  glBindTextureUnit(1, m_palettes.id());
  glBindVertexArray(m_vertexArray.id());
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
