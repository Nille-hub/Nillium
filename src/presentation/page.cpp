#include "presentation/page.h"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

// Doom's pixels are 1.2 times taller than wide: 320x200 pages show as 4:3,
// the 426x200 widescreen pages as 16:9.
constexpr float pixelAspect = 1.2f;

constexpr std::string_view vertexSource = R"glsl(
#version 460 core

out vec2 uv;

void main() {
  // Four corners of a rectangle covering the viewport, drawn as a triangle strip.
  vec2 corner = vec2(gl_VertexID & 1, gl_VertexID >> 1);
  // Texture row 0 is the top row of the graphic.
  uv = vec2(corner.x, 1.0 - corner.y);
  gl_Position = vec4(corner * 2.0 - 1.0, 0.0, 1.0);
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

// One texel per pixel: red = palette index, green = 255 if opaque, 0 if transparent.
std::vector<std::uint8_t> indexOpaqueTexels(const WadGraphic& graphic) {
  std::vector<std::uint8_t> texels;
  texels.reserve(graphic.indices.size() * 2);
  for (std::size_t i = 0; i < graphic.indices.size(); ++i) {
    texels.push_back(graphic.indices[i]);
    texels.push_back(graphic.opaque[i] != 0 ? 255 : 0);
  }
  return texels;
}

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

} // namespace

Page::Page(const WadGraphic& graphic, const std::array<Palette, paletteCount>& palettes)
    : m_program(vertexSource, fragmentSource),
      m_graphic(graphic.width, graphic.height, TextureFormat::Rg8, indexOpaqueTexels(graphic)),
      m_palettes(256, static_cast<int>(paletteCount), TextureFormat::Rgb8, paletteTexels(palettes)),
      m_aspect(static_cast<float>(graphic.width) / (static_cast<float>(graphic.height) * pixelAspect)) {}

void Page::draw(FramebufferSize size) const {
  glViewport(0, 0, size.width, size.height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Keep the page's aspect ratio and centre it in the window.
  const auto width = static_cast<float>(size.width);
  const auto height = static_cast<float>(size.height);
  int pageWidth = size.width;
  int pageHeight = size.height;
  if (width > height * m_aspect) {
    pageWidth = static_cast<int>(height * m_aspect);
  } else {
    pageHeight = static_cast<int>(width / m_aspect);
  }
  glViewport((size.width - pageWidth) / 2, (size.height - pageHeight) / 2, pageWidth, pageHeight);

  glUseProgram(m_program.id());
  glBindTextureUnit(0, m_graphic.id());
  glBindTextureUnit(1, m_palettes.id());
  glBindVertexArray(m_vertexArray.id());
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
