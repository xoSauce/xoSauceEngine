#ifndef MESH
#define MESH
#include <string>
#include <vector>

#include "Shader.h"
#include "Vertex.h"

namespace xo {
class Mesh {
 public:
  struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
  };
  std::vector<xo::Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<xo::Mesh::Texture> textures;

  Mesh(const std::vector<xo::Vertex>& vertices, const std::vector<unsigned int>& indices,
       const std::vector<xo::Mesh::Texture>& textures)
      : vertices{vertices}, indices{indices}, textures{textures} {
    setup();
  };
  void setup();
  void draw(xo::Shader&);

 private:
  unsigned int VBO, EBO, VAO;
};
}  // namespace xo
#endif