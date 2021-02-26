#ifndef MODEL
#define MODEL
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Mesh.h"
#include "Shader.h"

namespace xo {

class Model {
 public:
  struct AABB {
    AABB() : minAABB(glm::vec3(FLT_MAX)), maxAABB(glm::vec3{-FLT_MAX}) {};
    glm::vec3 minAABB;
    glm::vec3 maxAABB;
  } boundingBox;

  Model(const std::string &path, const glm::mat4 &worldMatrix = glm::mat4(1.0f));
  void draw(Shader &shader);
  void drawGizmo(Shader &shader);

  glm::mat4 worldMatrix;

 private:
  void processNode(const aiNode *rootNode, const aiScene *scene);
  void setupGizmo();
  Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
  unsigned int textureFromFile(const char *path, const std::string &directory);
  std::vector<xo::Mesh::Texture> loadMaterialTexture(aiMaterial *material, aiTextureType type, const char *typeName);
  std::vector<xo::Mesh> meshes;
  std::vector<xo::Mesh::Texture> textures_loaded;
  std::string directory;

  //Gizmo
  unsigned int gizmoVAO, gizmoVBO, gizmoEBO; 
};

}  // namespace xo
#endif