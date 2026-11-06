#include "presentation/worldrenderer.h"

#include "presentation/camera.h"
#include "presentation/worldgeometry.h"

#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <string_view>
#include <vector>

namespace {

struct SectorState {
  float floorHeight;
  float ceilingHeight;
  float light;
};

constexpr std::string_view vertexSource = R"glsl(
#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in uint front;
layout(location = 2) in uint back;
layout(location = 3) in uint part;
layout(location = 4) in float shade;

struct Sector {
  float floorHeight;
  float ceilingHeight;
  float light;
};

layout(std430, binding = 0) readonly buffer SectorStates {
  Sector sectors[];
};

layout(location = 0) uniform mat4 viewProjection;

flat out float brightness;

// Must match WorldPart in worldgeometry.h.
const uint FrontFloor = 0u;
const uint LowerBottom = 2u;
const uint LowerTop = 3u;
const uint UpperBottom = 4u;

void main() {
  Sector f = sectors[front];
  Sector b = sectors[back];

  float z;
  float light;
  if (part <= 1u) {
    z = (part == FrontFloor) ? f.floorHeight : f.ceilingHeight;
    light = f.light;
  } else if (part <= 3u) {
    z = (part == LowerBottom) ? min(f.floorHeight, b.floorHeight) : max(f.floorHeight, b.floorHeight);
    light = (f.floorHeight <= b.floorHeight) ? f.light : b.light;  // seen from the lower floor's side
  } else {
    z = (part == UpperBottom) ? min(f.ceilingHeight, b.ceilingHeight) : max(f.ceilingHeight, b.ceilingHeight);
    light = (f.ceilingHeight >= b.ceilingHeight) ? f.light : b.light;  // seen from the higher ceiling's side
  }

  brightness = light * shade;
  gl_Position = viewProjection * vec4(position, z, 1.0);
}
)glsl";

constexpr std::string_view fragmentSource = R"glsl(
#version 460 core

flat in float brightness;
out vec4 color;

void main() {
  color = vec4(vec3(brightness), 1.0);
}
)glsl";

constexpr GLuint bindingIndex = 0;
constexpr GLuint sectorStatesBinding = 0;
constexpr float maxLight = 255.0f;

} // namespace

WorldRenderer::WorldRenderer(const MapData& map) : m_program(vertexSource, fragmentSource) {
  const std::vector<WorldVertex> vertices = buildWorldGeometry(map);
  m_vertices.upload(std::as_bytes(std::span(vertices)));
  m_vertexCount = static_cast<GLsizei>(vertices.size());

  // Describe the vertex layout once: the VAO reads WorldVertex records from m_vertices.
  const GLuint vao = m_vertexArray.id();
  glVertexArrayVertexBuffer(vao, bindingIndex, m_vertices.id(), 0, sizeof(WorldVertex));

  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(WorldVertex, x));
  glVertexArrayAttribBinding(vao, 0, bindingIndex);

  // Integer attributes use the I variant, so the shader receives them unconverted.
  glEnableVertexArrayAttrib(vao, 1);
  glVertexArrayAttribIFormat(vao, 1, 1, GL_UNSIGNED_INT, offsetof(WorldVertex, front));
  glVertexArrayAttribBinding(vao, 1, bindingIndex);

  glEnableVertexArrayAttrib(vao, 2);
  glVertexArrayAttribIFormat(vao, 2, 1, GL_UNSIGNED_INT, offsetof(WorldVertex, back));
  glVertexArrayAttribBinding(vao, 2, bindingIndex);

  glEnableVertexArrayAttrib(vao, 3);
  glVertexArrayAttribIFormat(vao, 3, 1, GL_UNSIGNED_INT, offsetof(WorldVertex, part));
  glVertexArrayAttribBinding(vao, 3, bindingIndex);

  glEnableVertexArrayAttrib(vao, 4);
  glVertexArrayAttribFormat(vao, 4, 1, GL_FLOAT, GL_FALSE, offsetof(WorldVertex, shade));
  glVertexArrayAttribBinding(vao, 4, bindingIndex);
}

void WorldRenderer::draw(FramebufferSize window, const Viewpoint& viewpoint, std::span<const MapSector> sectors) {
  if (window.height == 0 || window.width == 0) {
    return;
  }

  std::vector<SectorState> sectorStates;
  sectorStates.reserve(sectors.size());

  for (const MapSector& sector : sectors) {
    sectorStates.push_back({static_cast<float>(sector.floorHeight), static_cast<float>(sector.ceilingHeight),
                            static_cast<float>(sector.light) / maxLight});
  }

  m_sectorStates.upload(std::as_bytes(std::span(sectorStates)));

  glViewport(0, 0, window.width, window.height);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glProgramUniformMatrix4fv(
    m_program.id(), 0, 1, GL_FALSE, glm::value_ptr(viewProjection(viewpoint, window)));
  glUseProgram(m_program.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sectorStatesBinding, m_sectorStates.id());
  glBindVertexArray(m_vertexArray.id());
  glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}
