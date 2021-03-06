#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec2 aTexCoords;
out vec3 position;
out vec4 color;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main() 
{
  gl_Position = projection * view * model * vec4(aPos, 1.0); 
  color = aCol;
  TexCoords = aTexCoords;
}

