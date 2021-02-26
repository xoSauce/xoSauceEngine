#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <stb_image.h>
#include <vector>

#include "Mesh.h"
#include "Vertex.h"
#define BITMASK_INDEX(idx, val) (int)(idx&val)!=0 

namespace xo {

void processNode(const aiNode *rootNode, const aiScene *scene);
Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
std::vector<xo::Mesh::Texture> loadMaterialTexture(aiMaterial *material, aiTextureType type, const char *typeName);
unsigned int textureFromFile(const char *path, const std::string &directory);

Model::Model(const std::string &path, const glm::mat4 &worldMatrix) : worldMatrix(worldMatrix) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

  if (!scene) {
    std::cout << "ASSIMP::ERROR IMPORTING FILE: " << path << ". Failed with error: " << importer.GetErrorString()
              << std::endl;
    return;
  }

  directory = path.substr(0, path.find_last_of("/"));
  if (scene->HasMeshes()) {
    processNode(scene->mRootNode, scene);
  }

  setupGizmo();
}

void Model::setupGizmo() {
  glm::vec3 *arr[2] = {&boundingBox.minAABB, &boundingBox.maxAABB};
  std::vector<glm::vec3> boundingBoxVertices;
  boundingBoxVertices.reserve(8);
  for (int i = 0; i < 8; ++i) {
    boundingBoxVertices.push_back(glm::vec3(arr[BITMASK_INDEX(i, 1)]->x, arr[BITMASK_INDEX(i, 2)]->y, arr[BITMASK_INDEX(i, 4)]->z));
  }
  std::vector<unsigned int> indices{
      0, 1, 2, 2, 3, 1,
      3, 7, 1, 1, 5, 7,
      5, 6, 4, 6, 5, 7,
      6, 0, 4, 0, 2, 6,
      6, 7, 3, 3, 2, 6,
      4, 5, 1, 1, 0, 4
  };

  glGenBuffers(1, &gizmoVBO);
  glGenBuffers(1, &gizmoEBO);
  glGenVertexArrays(1, &gizmoVAO);

  glBindVertexArray(gizmoVAO);
  glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
  glBufferData(GL_ARRAY_BUFFER, boundingBoxVertices.size() * sizeof(glm::vec3), &boundingBoxVertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gizmoEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

  glBindVertexArray(0);
}

void Model::draw(xo::Shader &shader) {
  shader.use();
  shader.setMat4("model", worldMatrix);
  for (auto &m : meshes) {
    m.draw(shader);
  }
}

void Model::drawGizmo(xo::Shader &shader) {
  // AABB
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader.use();
  shader.setMat4("model", worldMatrix);
  glBindVertexArray(gizmoVAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Model::processNode(const aiNode *rootNode, const aiScene *scene) {
  for (unsigned int i = 0; i < rootNode->mNumMeshes; ++i) {
    aiMesh *mesh = scene->mMeshes[rootNode->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < rootNode->mNumChildren; ++i) {
    processNode(rootNode->mChildren[i], scene);
  }
}

void updateBoundingBox(Model *model, const glm::vec3 &newPosition) {
  model->boundingBox.minAABB.x = std::min(model->boundingBox.minAABB.x, newPosition.x);
  model->boundingBox.minAABB.y = std::min(model->boundingBox.minAABB.y, newPosition.y);
  model->boundingBox.minAABB.z = std::min(model->boundingBox.minAABB.z, newPosition.z);
  model->boundingBox.maxAABB.x = std::max(model->boundingBox.maxAABB.x, newPosition.x);
  model->boundingBox.maxAABB.y = std::max(model->boundingBox.maxAABB.y, newPosition.y);
  model->boundingBox.maxAABB.z = std::max(model->boundingBox.maxAABB.z, newPosition.z);
}

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  vertices.reserve(mesh->mNumVertices);
  for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
    Vertex v;
    v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    updateBoundingBox(this, v.position);
    if (mesh->mTextureCoords[0]) {
      v.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    } else {
      v.texCoords = glm::vec2(0.0f);
    }
    vertices.push_back(v);
  }

  std::vector<unsigned int> indices;
  for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
    for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
      indices.push_back(mesh->mFaces[i].mIndices[j]);
    }
  }

  std::vector<xo::Mesh::Texture> textures;
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<xo::Mesh::Texture> diffuseMaps =
        loadMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<xo::Mesh::Texture> specularMaps =
        loadMaterialTexture(aiMaterial, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  return Mesh(vertices, indices, textures);
}

std::vector<xo::Mesh::Texture> Model::loadMaterialTexture(aiMaterial *material, aiTextureType type,
                                                          const char *typeName) {
  std::vector<xo::Mesh::Texture> textures;
  for (unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
    aiString str;
    material->GetTexture(type, i, &str);
    bool skip = false;

    for (unsigned int j = 0; j < textures_loaded.size(); ++j) {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      xo::Mesh::Texture texture;
      texture.id = textureFromFile(str.C_Str(), directory);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(texture);
    }
  }
  return textures;
}

unsigned int Model::textureFromFile(const char *path, const std::string &directory) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;
  unsigned int textureID;
  glGenTextures(1, &textureID);
  int width, height, nrComponents;
  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1) {
      format = GL_RED;
    } else if (nrComponents == 3) {
      format = GL_RGB;
    } else if (nrComponents == 4) {
      format = GL_RGBA;
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cout << "Texture failed to load at path: " << path << ", filename: " << filename << std::endl;
  }
  stbi_image_free(data);
  return textureID;
}
}  // namespace xo