#include "presentation/texturearray.h"
#include "presentation/texel.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

TextureArray::TextureArray(std::span<const WadGraphic> graphics) {
  if (graphics.empty()) {
    throw std::invalid_argument(
      "trying to construct TextureArray instance from empty graphics vector");
  }

  int maxWidth = std::numeric_limits<int>::min();
  int maxHeight = std::numeric_limits<int>::min();
  m_layerSizes.reserve(graphics.size());

  for (const WadGraphic& graphic : graphics) {
    maxWidth = std::max(maxWidth, graphic.width);
    maxHeight = std::max(maxHeight, graphic.height);

    m_layerSizes.push_back({ graphic.width, graphic.height });
  }

  glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_id);
  glTextureStorage3D(m_id, 1, GL_RG8, maxWidth, maxHeight, static_cast<GLsizei>(m_layerSizes.size()));
  glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (std::size_t i = 0; i < graphics.size(); ++i) {
    const WadGraphic& graphic = graphics[i];
    const std::vector<std::uint8_t> texels = indexOpaqueTexels(graphic);

    glTextureSubImage3D(m_id, 0, 0, 0, static_cast<GLint>(i), graphic.width, graphic.height, 1,
                        GL_RG, GL_UNSIGNED_BYTE, texels.data());
  }
}

TextureArray::~TextureArray() {
  glDeleteTextures(1, &m_id);
}

TextureArray::TextureArray(TextureArray&& other) noexcept
  : m_id(std::exchange(other.m_id, 0)), m_layerSizes(std::exchange(other.m_layerSizes, {})) {}

TextureArray& TextureArray::operator=(TextureArray&& other) noexcept {
  if (this != &other) {
    glDeleteTextures(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
    m_layerSizes = std::exchange(other.m_layerSizes, {});
  }
  return *this;
}

GLuint TextureArray::id() const {
  return m_id;
}

const std::vector<LayerSize>& TextureArray::layerSizes() const {
  return m_layerSizes;
}
