#include "presentation/texture.h"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace {

struct FormatInfo {
  std::size_t channels;
  GLenum storageFormat;
  GLenum dataFormat;
};

FormatInfo formatInfo(TextureFormat format) {
  switch (format) {
  case TextureFormat::R8:   return {1, GL_R8, GL_RED};
  case TextureFormat::Rg8:  return {2, GL_RG8, GL_RG};
  case TextureFormat::Rgb8: return {3, GL_RGB8, GL_RGB};
  }
  throw std::invalid_argument("Texture: unknown format");
}

} // namespace

Texture::Texture(int width, int height, TextureFormat format, std::span<const std::uint8_t> pixels) {
  const FormatInfo info = formatInfo(format);
  const std::size_t channels = info.channels;
  if (width <= 0 || height <= 0 ||
      pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * channels) {
    throw std::invalid_argument("Texture: pixel data does not match width, height and format");
  }

  const GLenum storageFormat = info.storageFormat;
  const GLenum dataFormat = info.dataFormat;

  glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
  glTextureStorage2D(m_id, 1, storageFormat, width, height);
  glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTextureSubImage2D(m_id, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, pixels.data());
}

Texture::~Texture() {
  glDeleteTextures(1, &m_id);
}

Texture::Texture(Texture&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}

Texture& Texture::operator=(Texture&& other) noexcept {
  if (this != &other) {
    glDeleteTextures(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
  }
  return *this;
}

GLuint Texture::id() const {
  return m_id;
}
