#pragma once

#include "assets/map.h"
#include "assets/wad.h"
#include "game/game.h"
#include "platform/video.h"
#include "presentation/buffer.h"
#include "presentation/leveltextures.h"
#include "presentation/shaderprogram.h"
#include "presentation/texture.h"
#include "presentation/vertexarray.h"

#include <glad/glad.h>

#include <span>

// Draws the level's walls, floors and ceilings in 3D. The geometry is built once from the
// map; heights, light levels and textures are uploaded on every draw.
class WorldRenderer {
public:
  WorldRenderer(const Wad& wad, const MapData& map);

  // Not const: each call replaces the sector and sidedef state buffers' contents.
  void draw(FramebufferSize window, const Viewpoint& viewpoint, const Texture& palettes, int levelTime,
            std::span<const MapSector> sectors, std::span<const MapSidedef> sidedefs);

private:
  ShaderProgram m_program;
  LevelTextures m_textures;
  Buffer m_vertices;
  Buffer m_sectorStates;
  Buffer m_sidedefStates;
  Buffer m_layerSizes;
  Buffer m_translations;
  VertexArray m_vertexArray;
  GLsizei m_vertexCount = 0;
};
