#include "presentation/worldrenderer.h"
#include "assets/colormap.h"
#include "presentation/camera.h"
#include "presentation/worldgeometry.h"
#include "game/animations.h"

#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numeric>
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
layout(location = 9) in vec2 direction;

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
layout(std430, binding = 3) readonly buffer Translations { uint translations[]; };

layout(location = 0) uniform mat4 viewProjection;
layout(location = 1) uniform vec2 viewer;         // the viewpoint's map position
layout(location = 2) uniform vec2 viewDirection;  // unit vector the viewpoint looks along

flat out uint layer;
flat out int lightStep;  // 0 (dark) to 15 (bright), as vanilla's lightnum
flat out int isWall;     // walls and flats use different distance tables
out vec2 texel;
out float depth;         // distance along the view direction, which vanilla's lighting uses

// Must match WorldPart in worldgeometry.h.
const uint FrontFloor = 0u;
const uint FrontCeiling = 1u;
const uint WallBottom = 2u;
const uint WallTop = 3u;
const uint LowerBottom = 4u;
const uint LowerTop = 5u;
const uint UpperBottom = 6u;
const uint UpperTop = 7u;
const uint MiddleBottom = 8u;
const uint MiddleTop = 9u;

// Must match noLayer below and the flags in map.h.
const uint noLayer = 0xFFFFFFFFu;
const uint upperUnpegged = 0x0008u;
const uint lowerUnpegged = 0x0010u;

// The layer an animated texture currently shows; all other layers map to themselves.
uint currentLayer(uint l) {
  return l == noLayer ? noLayer : translations[l];
}

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
    layer = currentLayer((part == FrontFloor) ? f.floorLayer : f.ceilingLayer);
    lightStep = int(f.light) >> 4;
    isWall = 0;
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
      layer = currentLayer(side.middleLayer);
      anchor = ((flags & lowerUnpegged) != 0u) ? bottom + textureHeight(layer) : top;
    } else if (part == LowerBottom || part == LowerTop) {
      frontVisible = f.floorHeight <= b.floorHeight;  // seen from the lower floor's side
      side = sidedefs[frontVisible ? frontSide : backSide];
      bottom = min(f.floorHeight, b.floorHeight);
      top = max(f.floorHeight, b.floorHeight);
      layer = currentLayer(side.lowerLayer);
      float visibleCeiling = frontVisible ? f.ceilingHeight : b.ceilingHeight;
      anchor = ((flags & lowerUnpegged) != 0u) ? visibleCeiling : top;
    } else if (part == UpperBottom || part == UpperTop) {
      frontVisible = f.ceilingHeight >= b.ceilingHeight;  // seen from the higher ceiling's side
      side = sidedefs[frontVisible ? frontSide : backSide];
      bottom = min(f.ceilingHeight, b.ceilingHeight);
      top = max(f.ceilingHeight, b.ceilingHeight);
      layer = currentLayer(side.upperLayer);
      anchor = ((flags & upperUnpegged) != 0u) ? top : bottom + textureHeight(layer);
    } else {
      // Two-sided middle texture: seen from the side the viewer is on (Doom's front side is the
      // right side of the line), one texture height tall, clipped to the opening.
      vec2 lineStart = position - direction * along;
      vec2 toViewer = viewer - lineStart;
      frontVisible = direction.x * toViewer.y - direction.y * toViewer.x < 0.0;
      side = sidedefs[frontVisible ? frontSide : backSide];
      layer = currentLayer(side.middleLayer);
      float openingBottom = max(f.floorHeight, b.floorHeight);
      float openingTop = min(f.ceilingHeight, b.ceilingHeight);
      anchor = ((flags & lowerUnpegged) != 0u) ? openingBottom + textureHeight(layer) : openingTop;
      float textureTop = anchor + side.yOffset;
      bottom = max(textureTop - textureHeight(layer), openingBottom);
      top = max(min(textureTop, openingTop), bottom);
    }

    bool isBottom = (part == WallBottom || part == LowerBottom || part == UpperBottom ||
                     part == MiddleBottom);
    z = isBottom ? bottom : top;
    // Vanilla's fake contrast: east-west walls one step darker, north-south walls one brighter.
    int contrast = (direction.y == 0.0) ? -1 : ((direction.x == 0.0) ? 1 : 0);
    lightStep = clamp((int(frontVisible ? f.light : b.light) >> 4) + contrast, 0, 15);
    isWall = 1;
    float u = (frontVisible ? along : lineLength - along) + side.xOffset;
    float v = anchor - z + side.yOffset;
    texel = vec2(u, v);
  }

  depth = dot(position - viewer, viewDirection);
  gl_Position = viewProjection * vec4(position, z, 1.0);
}
)glsl";

constexpr std::string_view fragmentSource = R"glsl(
#version 460 core

flat in uint layer;
flat in int lightStep;
flat in int isWall;
in vec2 texel;
in float depth;
out vec4 color;

struct LayerSize {
  int width;
  int height;
};

layout(std430, binding = 2) readonly buffer LayerSizes { LayerSize layerSizes[]; };

layout(binding = 0) uniform sampler2DArray textures;
layout(binding = 1) uniform sampler2D palettes;
layout(binding = 2) uniform sampler2D colormap;

// Vanilla's light tables (r_main.c): a row starting at startmap, brightened up close.
int colormapRow() {
  int startmap = (15 - lightStep) * 4;
  float d = max(depth, 1.0);
  int brighten;
  if (isWall != 0) {
    brighten = min(int(2560.0 / d), 47) / 2;  // scalelight: scale index 2560 / depth, at most 47
  } else {
    int j = min(int(d / 16.0), 127);          // zlight: depth in steps of 16, at most 127
    brighten = (160 / (j + 1)) / 2;
  }
  return clamp(startmap - brighten, 0, 31);
}

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
  int lit = int(texelFetch(colormap, ivec2(index, colormapRow()), 0).r * 255.0 + 0.5);
  color = vec4(texelFetch(palettes, ivec2(lit, 0), 0).rgb, 1.0);
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
constexpr GLuint translationsBinding = 3;
constexpr GLuint textureArrayUnit = 0;
constexpr GLuint paletteUnit = 1;
constexpr GLuint colormapUnit = 2;
constexpr std::uint32_t noLayer = 0xFFFFFFFF;

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
    : m_program(vertexSource, fragmentSource),
      m_textures(wad, map),
      m_colormap(256, static_cast<int>(colormapRows), TextureFormat::R8, readColormap(wad).indices) {
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
  floatAttribute(9, 2, offsetof(WorldVertex, directionX));

  // The layers never change size, so their sizes are uploaded once.
  const std::vector<LayerSize>& sizes = m_textures.array().layerSizes();
  m_layerSizes.upload(std::as_bytes(std::span(sizes)));
}

void WorldRenderer::draw(FramebufferSize window, const Viewpoint& viewpoint,
                         const Texture& palettes, int levelTime,
                         std::span<const MapSector> sectors, std::span<const MapSidedef> sidedefs) {
  if (window.width <= 0 || window.height <= 0) {
    return;
  }

  std::vector<SectorState> sectorStates;
  sectorStates.reserve(sectors.size());
  for (const MapSector& sector : sectors) {
    sectorStates.push_back({static_cast<float>(sector.floorHeight),
                            static_cast<float>(sector.ceilingHeight),
                            static_cast<float>(sector.light),
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

  // Every layer shows itself, except animated ones, which show their group's current frame.
  std::vector<std::uint32_t> translations(m_textures.array().layerSizes().size());
  std::iota(translations.begin(), translations.end(), std::uint32_t{0});
  const auto step = static_cast<std::size_t>(levelTime / ticsPerAnimationFrame);
  for (const std::vector<std::uint32_t>& group : m_textures.animations()) {
    for (std::size_t k = 0; k < group.size(); ++k) {
      translations[group[k]] = group[(step + k) % group.size()];
    }
  }
  m_translations.upload(std::as_bytes(std::span(translations)));

  glViewport(0, 0, window.width, window.height);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  const glm::mat4 matrix = viewProjection(viewpoint, window);
  glProgramUniformMatrix4fv(m_program.id(), 0, 1, GL_FALSE, glm::value_ptr(matrix));
  glProgramUniform2f(m_program.id(), 1, viewpoint.x, viewpoint.y);
  glProgramUniform2f(m_program.id(), 2, std::cos(viewpoint.angle), std::sin(viewpoint.angle));
  glUseProgram(m_program.id());
  glBindTextureUnit(textureArrayUnit, m_textures.array().id());
  glBindTextureUnit(paletteUnit, palettes.id());
  glBindTextureUnit(colormapUnit, m_colormap.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sectorStatesBinding, m_sectorStates.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sidedefStatesBinding, m_sidedefStates.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, layerSizesBinding, m_layerSizes.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, translationsBinding, m_translations.id());
  glBindVertexArray(m_vertexArray.id());
  glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}
