#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>

#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <string.h>

namespace xo {
class Shader {
 public:
  GLuint ID;
  Shader(const char *vertexPath, const char *fragmentPath) {
    char line[16384], vertexCode[16384], fragmentCode[16384];
    char *vertexCodeP, *fragmentCodeP;
    vertexCodeP = fragmentCodeP = NULL;
    FILE *vertexPathFp = fopen(vertexPath, "r+");
    // Read VertexPath
    fgets(line, sizeof(line), vertexPathFp);
    strcpy(vertexCode, line);
    while (fgets(line, sizeof(line), vertexPathFp)) {
      strcat(vertexCode, "\n");
      strcat(vertexCode, line);
    }
    strcat(vertexCode, "\0");
    fclose(vertexPathFp);
    vertexCodeP = vertexCode;
    // end vertexPath

    // Read FragmentPath
    FILE *fragmentPathFp = fopen(fragmentPath, "r+");
    fgets(line, sizeof(line), fragmentPathFp);
    strcpy(fragmentCode, line);
    while (fgets(line, sizeof(line), fragmentPathFp)) {
      strcat(fragmentCode, "\n");
      strcat(fragmentCode, line);
    }
    strcat(fragmentCode, "\0");
    fclose(fragmentPathFp);
    fragmentCodeP = fragmentCode;

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCodeP, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s \n", infoLog);
    }
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCodeP, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s \n", infoLog);
    }
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(fragment, 512, NULL, infoLog);
      printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s \n", infoLog);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  void use() { glUseProgram(ID); }

  void setBool(const char *name, bool value) const { glUniform1i(glGetUniformLocation(ID, name), value); }

  void setInt(const char *name, int value) const { glUniform1i(glGetUniformLocation(ID, name), value); }

  void setFloat(const char *name, float value) const { glUniform1f(glGetUniformLocation(ID, name), value); }
  void setMat4(const char *name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
  }

  void setVec3(const char *name, const glm::vec3 &vec) const {
    glUniform3fv(glGetUniformLocation(ID, name), 1, &vec[0]);
  }

  void setVec3(const char *name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
  }

  void setVec4(const char *name, const glm::vec4 &vec) const {
    glUniform4fv(glGetUniformLocation(ID, name), 1, &vec[0]);
  }
};

}  // namespace xo
#endif