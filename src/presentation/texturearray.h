#pragma once

#include "assets/graphic.h"

#include <glad/glad.h>

#include <span>
#include <vector>

// One layer's real size within the array; layers are as large as the largest graphic.
struct LayerSize {
  int width = 0;
  int height = 0;
};

// A GL texture array with one layer per graphic: palette index and opaque flag per texel
// (GL_RG8), each graphic in its layer's top-left corner. Built once, immutable afterwards.
class TextureArray {
public:
  explicit TextureArray(std::span<const WadGraphic> graphics);
  ~TextureArray();

  TextureArray(const TextureArray&) = delete;
  TextureArray& operator=(const TextureArray&) = delete;
  TextureArray(TextureArray&& other) noexcept;
  TextureArray& operator=(TextureArray&& other) noexcept;

  GLuint id() const;
  const std::vector<LayerSize>& layerSizes() const;

private:
  GLuint m_id = 0;
  std::vector<LayerSize> m_layerSizes;
};
