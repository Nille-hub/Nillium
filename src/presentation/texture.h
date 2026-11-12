#pragma once

#include <glad/glad.h>

#include <cstdint>
#include <span>

enum class TextureFormat {
  R8,    // one byte per texel, e.g. colormap rows of palette indices
  Rg8,   // palette index and opaque flag
  Rgb8,  // colours, e.g. the palettes
};

class Texture {
public:
  Texture(int width, int height, TextureFormat format, std::span<const std::uint8_t> pixels);
  ~Texture();

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  GLuint id() const;

private:
  GLuint m_id = 0;
};
