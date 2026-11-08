#include "presentation/worldrenderer.h"

#include "presentation/camera.h"
#include "presentation/worldgeometry.h"

#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::string_view vertexSource = R"glsl(
#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in float along;
layout(location = 2) in float lineLength;
layout(location = 3) in uint front;
layout(location = 4) in uint back;
layout(location = 5) in uint frontSide;
layout(location = 6) in uint backSide;
layout(location = 7) in uint flags;
layout(location = 8) in uint part;

struct Sector {
  float floorHeight;
  float ceilingHeight;
  float light;
  uint floorLayer;
  uint ceilingLayer;
};

struct Sidedef {
  uint upperLayer;
  uint middleLayer;
  uint lowerLayer;
  float xOffset;
  float yOffset;
};

struct LayerSize {
  int width;
  int height;
};

layout(std430, binding = 0) readonly buffer SectorStates { Sector sectors[]; };
layout(std430, binding = 1) readonly buffer SidedefStates { Sidedef sidedefs[]; };
layout(std430, binding = 2) readonly buffer LayerSizes { LayerSize layerSizes[]; };

layout(location = 0) uniform mat4 viewProjection;

flat out uint layer;
flat out float brightness;
out vec2 texel;

// Must match WorldPart in worldgeometry.h.
const uint FrontFloor = 0u;
const uint FrontCeiling = 1u;
const uint WallBottom = 2u;
const uint WallTop = 3u;
const uint LowerBottom = 4u;
const uint LowerTop = 5u;
const uint UpperBottom = 6u;
const uint UpperTop = 7u;

// Must match noLayer below and the flags in map.h.
const uint noLayer = 0xFFFFFFFFu;
const uint upperUnpegged = 0x0008u;
const uint lowerUnpegged = 0x0010u;

float textureHeight(uint l) {
  return l == noLayer ? 0.0 : float(layerSizes[l].height);
}

void main() {
  Sector f = sectors[front];
  Sector b = sectors[back];
  float z;

  if (part == FrontFloor || part == FrontCeiling) {
    // Flats lie on vanilla's fixed 64-unit world grid; texture rows run southward.
    z = (part == FrontFloor) ? f.floorHeight : f.ceilingHeight;
    layer = (part == FrontFloor) ? f.floorLayer : f.ceilingLayer;
    brightness = f.light;
    texel = vec2(position.x, -position.y);
  } else {
    bool frontVisible;
    float bottom;
    float top;
    float anchor;  // world height of the texture's top row
    Sidedef side;

    if (part == WallBottom || part == WallTop) {
      frontVisible = true;
      side = sidedefs[frontSide];
      bottom = f.floorHeight;
      top = f.ceilingHeight;
      layer = side.middleLayer;
      anchor = ((flags & lowerUnpegged) != 0u) ? bottom + textureHeight(layer) : top;
    } else if (part == LowerBottom || part == LowerTop) {
      frontVisible = f.floorHeight <= b.floorHeight;  // seen from the lower floor's side
      side = sidedefs[frontVisible ? frontSide : backSide];
      bottom = min(f.floorHeight, b.floorHeight);
      top = max(f.floorHeight, b.floorHeight);
      layer = side.lowerLayer;
      float visibleCeiling = frontVisible ? f.ceilingHeight : b.ceilingHeight;
      anchor = ((flags & lowerUnpegged) != 0u) ? visibleCeiling : top;
    } else {
      frontVisible = f.ceilingHeight >= b.ceilingHeight;  // seen from the higher ceiling's side
      side = sidedefs[frontVisible ? frontSide : backSide];
      bottom = min(f.ceilingHeight, b.ceilingHeight);
      top = max(f.ceilingHeight, b.ceilingHeight);
      layer = side.upperLayer;
      anchor = ((flags & upperUnpegged) != 0u) ? top : bottom + textureHeight(layer);
    }

    bool isBottom = (part == WallBottom || part == LowerBottom || part == UpperBottom);
    z = isBottom ? bottom : top;
    brightness = frontVisible ? f.light : b.light;
    float u = (frontVisible ? along : lineLength - along) + side.xOffset;
    float v = anchor - z + side.yOffset;
    texel = vec2(u, v);
  }

  gl_Position = viewProjection * vec4(position, z, 1.0);
}
)glsl";

constexpr std::string_view fragmentSource = R"glsl(
#version 460 core

flat in uint layer;
flat in float brightness;
in vec2 texel;
out vec4 color;

struct LayerSize {
  int width;
  int height;
};

layout(std430, binding = 2) readonly buffer LayerSizes { LayerSize layerSizes[]; };

layout(binding = 0) uniform sampler2DArray textures;
layout(binding = 1) uniform sampler2D palettes;

void main() {
  if (layer == 0xFFFFFFFFu) {
    discard;  // "-": vanilla draws nothing here
  }
  vec2 size = vec2(layerSizes[layer].width, layerSizes[layer].height);
  ivec2 position = ivec2(mod(floor(texel), size));  // repeat within the texture's own size
  vec2 indexOpaque = texelFetch(textures, ivec3(position, int(layer)), 0).rg;
  if (indexOpaque.g < 0.5) {
    discard;
  }
  int index = int(indexOpaque.r * 255.0 + 0.5);
  color = vec4(texelFetch(palettes, ivec2(index, 0), 0).rgb * brightness, 1.0);
}
)glsl";

struct SectorState {
  float floorHeight;
  float ceilingHeight;
  float light;
  std::uint32_t floorLayer;
  std::uint32_t ceilingLayer;
};

struct SidedefState {
  std::uint32_t upperLayer;
  std::uint32_t middleLayer;
  std::uint32_t lowerLayer;
  float xOffset;
  float yOffset;
};

constexpr GLuint bindingIndex = 0;
constexpr GLuint sectorStatesBinding = 0;
constexpr GLuint sidedefStatesBinding = 1;
constexpr GLuint layerSizesBinding = 2;
constexpr GLuint textureArrayUnit = 0;
constexpr GLuint paletteUnit = 1;
constexpr std::uint32_t noLayer = 0xFFFFFFFF;
constexpr float maxLight = 255.0f;

std::uint32_t wallLayerOrNone(const LevelTextures& textures, const std::string& name) {
  return name != noTexture ? textures.wallLayer(name) : noLayer;
}

SidedefState sidedefState(const LevelTextures& textures, const MapSidedef& sidedef) {
  return {wallLayerOrNone(textures, sidedef.upperTexture), wallLayerOrNone(textures, sidedef.middleTexture),
          wallLayerOrNone(textures, sidedef.lowerTexture), static_cast<float>(sidedef.xOffset),
          static_cast<float>(sidedef.yOffset)};
}

} // namespace

WorldRenderer::WorldRenderer(const Wad& wad, const MapData& map)
    : m_program(vertexSource, fragmentSource), m_textures(wad, map) {
  const std::vector<WorldVertex> vertices = buildWorldGeometry(map);
  m_vertices.upload(std::as_bytes(std::span(vertices)));
  m_vertexCount = static_cast<GLsizei>(vertices.size());

  // Describe the vertex layout once: the VAO reads WorldVertex records from m_vertices.
  const GLuint vao = m_vertexArray.id();
  glVertexArrayVertexBuffer(vao, bindingIndex, m_vertices.id(), 0, sizeof(WorldVertex));

  const auto floatAttribute = [&](GLuint index, GLint count, std::size_t offset) {
    glEnableVertexArrayAttrib(vao, index);
    glVertexArrayAttribFormat(vao, index, count, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offset));
    glVertexArrayAttribBinding(vao, index, bindingIndex);
  };
  // Integer attributes use the I variant, so the shader receives them unconverted.
  const auto uintAttribute = [&](GLuint index, std::size_t offset) {
    glEnableVertexArrayAttrib(vao, index);
    glVertexArrayAttribIFormat(vao, index, 1, GL_UNSIGNED_INT, static_cast<GLuint>(offset));
    glVertexArrayAttribBinding(vao, index, bindingIndex);
  };

  floatAttribute(0, 2, offsetof(WorldVertex, x));
  floatAttribute(1, 1, offsetof(WorldVertex, along));
  floatAttribute(2, 1, offsetof(WorldVertex, lineLength));
  uintAttribute(3, offsetof(WorldVertex, front));
  uintAttribute(4, offsetof(WorldVertex, back));
  uintAttribute(5, offsetof(WorldVertex, frontSide));
  uintAttribute(6, offsetof(WorldVertex, backSide));
  uintAttribute(7, offsetof(WorldVertex, flags));
  uintAttribute(8, offsetof(WorldVertex, part));

  // The layers never change size, so their sizes are uploaded once.
  const std::vector<LayerSize>& sizes = m_textures.array().layerSizes();
  m_layerSizes.upload(std::as_bytes(std::span(sizes)));
}

void WorldRenderer::draw(FramebufferSize window, const Viewpoint& viewpoint, const Texture& palettes,
                         std::span<const MapSector> sectors, std::span<const MapSidedef> sidedefs) {
  if (window.width <= 0 || window.height <= 0) {
    return;
  }

  std::vector<SectorState> sectorStates;
  sectorStates.reserve(sectors.size());
  for (const MapSector& sector : sectors) {
    sectorStates.push_back({static_cast<float>(sector.floorHeight),
                            static_cast<float>(sector.ceilingHeight),
                            static_cast<float>(sector.light) / maxLight,
                            m_textures.flatLayer(sector.floorFlat),
                            m_textures.flatLayer(sector.ceilingFlat)});
  }

  std::vector<SidedefState> sidedefStates;
  sidedefStates.reserve(sidedefs.size());
  for (const MapSidedef& sidedef : sidedefs) {
    sidedefStates.push_back(sidedefState(m_textures, sidedef));
  }

  m_sectorStates.upload(std::as_bytes(std::span(sectorStates)));
  m_sidedefStates.upload(std::as_bytes(std::span(sidedefStates)));

  glViewport(0, 0, window.width, window.height);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  const glm::mat4 matrix = viewProjection(viewpoint, window);
  glProgramUniformMatrix4fv(m_program.id(), 0, 1, GL_FALSE, glm::value_ptr(matrix));
  glUseProgram(m_program.id());
  glBindTextureUnit(textureArrayUnit, m_textures.array().id());
  glBindTextureUnit(paletteUnit, palettes.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sectorStatesBinding, m_sectorStates.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sidedefStatesBinding, m_sidedefStates.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, layerSizesBinding, m_layerSizes.id());
  glBindVertexArray(m_vertexArray.id());
  glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}
