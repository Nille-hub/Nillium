#include "presentation/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace {

constexpr float nearPlane = 4.0f;
constexpr float farPlane = 32768.0f;
constexpr float halfFovTangent = 0.75f;


} // namespace

glm::mat4 viewProjection(const Viewpoint& viewpoint, FramebufferSize window) {
  if (window.width <= 0 || window.height <= 0) {
    return glm::mat4(1.0f);
  }
  const float verticalFov = 2.0f * std::atan(halfFovTangent);
  const float aspect = static_cast<float>(window.width) / window.height;
  glm::vec3 eye(viewpoint.x, viewpoint.y, viewpoint.z);
  glm::vec3 forw(std::cos(viewpoint.angle), std::sin(viewpoint.angle), 0.0f);
  glm::vec3 up(0.0f, 0.0f, 1.0f);

  return glm::perspective(verticalFov, aspect, nearPlane, farPlane)
       * glm::lookAt(eye, eye + forw, up);
}
