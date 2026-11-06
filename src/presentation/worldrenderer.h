#pragma once

#include "assets/map.h"
#include "game/game.h"
#include "platform/video.h"
#include "presentation/buffer.h"
#include "presentation/shaderprogram.h"
#include "presentation/vertexarray.h"

#include <glad/glad.h>

#include <span>

// Draws the level's walls, floors and ceilings in 3D. The geometry is built once from the
// map; the sector heights and light levels are uploaded on every draw.
class WorldRenderer {
public:
  explicit WorldRenderer(const MapData& map);

  // Not const: each call replaces the sector state buffer's contents.
  void draw(FramebufferSize window, const Viewpoint& viewpoint, std::span<const MapSector> sectors);

private:
  ShaderProgram m_program;
  Buffer m_vertices;
  Buffer m_sectorStates;
  VertexArray m_vertexArray;
  GLsizei m_vertexCount = 0;
};
