#include "Mesh.h"

#include <SDL2/SDL_opengl.h>
#include <string>
#include <vector>

#include "Shader.h"

namespace xo {
void Mesh::setup() {
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
  glBindVertexArray(0);
}

void Mesh::draw(xo::Shader& shader) {
  const char* TEXTURE_DIFFUSE_NAME = "texture_diffuse";
  const char* TEXTURE_SPECULAR_NAME = "texture_specular";
  unsigned int diffuseCount = 1;
  unsigned int specularCount = 1;
  for (unsigned int i = 0; i < textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;
    if (strcmp(name.c_str(), TEXTURE_DIFFUSE_NAME) == 0) {
      number = std::to_string(diffuseCount++);
    } else if (strcmp(name.c_str(), TEXTURE_SPECULAR_NAME) == 0) {
      number = std::to_string(specularCount++);
    }
    shader.setFloat((name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

}  // namespace xo