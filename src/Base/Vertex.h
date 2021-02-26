#ifndef VERTEX
#define VERTEX
#include <glm/glm.hpp>

namespace xo
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoords;
  };
} // namespace xo
#endif