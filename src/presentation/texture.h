#pragma once

#include <glad/glad.h>

#include <cstdint>
#include <span>

enum class TextureFormat {
  Rg8,
  Rgb8,
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
