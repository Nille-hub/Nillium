#pragma once

#include "assets/map.h"
#include "assets/wad.h"
#include "game/game.h"
#include "platform/video.h"
#include "presentation/buffer.h"
#include "presentation/leveltextures.h"
#include "presentation/shaderprogram.h"
#include "presentation/vertexarray.h"

#include <glad/glad.h>

#include <span>

// Draws the level's walls, floors and ceilings in 3D. The geometry is built once from the
// map; the sector heights and light levels are uploaded on every draw.
class WorldRenderer {
public:
  WorldRenderer(const Wad& wad, const MapData& map);

  // Not const: each call replaces the sector state buffer's contents.
  void draw(FramebufferSize window, const Viewpoint& viewpoint, std::span<const MapSector> sectors,
            std::span<const MapSidedef> sidedefs);

private:
  ShaderProgram m_program;
  LevelTextures m_textures;
  Buffer m_vertices;
  Buffer m_sectorStates;
  Buffer m_sidedefStates;
  VertexArray m_vertexArray;
  GLsizei m_vertexCount = 0;
};
