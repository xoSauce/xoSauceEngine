#include <glad/glad.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/imgui.h>
#include <iostream>
#include <stdio.h>

#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Vertex.h"

#define GLSL_VERSION "#version 460"

xo::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime, lastFrame;
void pollInputEvent();
void pollKeyboard();
GLsizei SCR_WIDTH = 1920, SCR_HEIGHT = 1080;
SDL_GLContext gContext;
GLuint gProgramID, VBO, VAO, VIB;
bool shouldRun = true;

unsigned int indices[] = {0, 1, 2, 1, 3, 2};

float frand() {
  float f = rand() / (float)RAND_MAX;
  std::cout << "RANDOM: " << f << std::endl;
  return f;
}

void initGL() {
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  // srand(SDL_GetTicks());
  // xo::Vertex vertices[4];
  // vertices[0].position = glm::vec3(-5.0f, -1.0f, -5.0f);
  // vertices[0].color = glm::vec4(frand(), frand(), frand(), 1.0f);

  // vertices[1].position = glm::vec3(5.0f, -1.0f, -5.0f);
  // vertices[1].color = glm::vec4(frand(), frand(), frand(), 1.0f);

  // vertices[2].position = glm::vec3(-5.0f, -1.0f, 5.0f);
  // vertices[2].color = glm::vec4(frand(), frand(), frand(), 1.0f);

  // vertices[3].position = glm::vec3(5.0f, -1.0f, 5.0f);
  // vertices[3].color = glm::vec4(frand(), frand(), frand(), 1.0f);

  // glGenVertexArrays(1, &VAO);
  // glBindVertexArray(VAO);
  // glGenBuffers(1, &VBO);
  // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // glEnableVertexAttribArray(0);
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(xo::Vertex), (void*)offsetof(xo::Vertex, position));
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(xo::Vertex), (void*)offsetof(xo::Vertex, color));

  // glGenBuffers(1, &VIB);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VIB);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glEnable(GL_DEPTH_TEST);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBindVertexArray(0);
}

int main(int argc, char* argv[]) {
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL COULD NOT INITIALIZE! SDL_ERROR %s\n", SDL_GetError());
    success = false;
  }
  // SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_Window* gWindow = SDL_CreateWindow("XoSauce", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH,
                                         SCR_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  gContext = SDL_GL_CreateContext(gWindow);
  SDL_GL_MakeCurrent(gWindow, gContext);
  if (gContext == NULL) {
    std::cout << "OpenGL Context could not be created! SDL ERROR: " << SDL_GetError() << std::endl;
    success = false;
  }
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cout << "GLAD ProcAddress could not loaded! SDL ERROR: " << SDL_GetError() << std::endl;
    success = false;
  }

  // Platform Asserts
  static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support structure padding.");
  static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4, "Platform doesn't support structure padding.");

  // setup IMGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
  ImGui_ImplOpenGL3_Init(GLSL_VERSION);
  //

  bool showAnotherWindow = true;
  xo::Shader gExampleShader("shaders/example.vs", "shaders/example.fs");
  xo::Shader gizmoShader("shaders/example.vs", "shaders/gizmo.fs");
  initGL();
  lastFrame = SDL_GetTicks();
  float modelPosition[4] = {0.0f, 0.0f, 0.0f, 1.0f};

  float scaleFactor = 0.01f;
  // xo::Model backpackOBJ("res/among-us-fbx/amongus.FBX", glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor,
  // scaleFactor, scaleFactor)));
  xo::Model backpackOBJ("res/backpack/backpack.obj");
  glBindVertexArray(VAO);
  while (shouldRun) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(gWindow);
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    if (showAnotherWindow) {
      ImGui::Begin("Model Window", &showAnotherWindow);
      ImGui::Text("Hello from another window !");
      ImGui::InputFloat3("MIN_AABB", glm::value_ptr(backpackOBJ.boundingBox.minAABB));
      ImGui::InputFloat3("MAX_AABB", glm::value_ptr(backpackOBJ.boundingBox.maxAABB));
      ImGui::DragFloat3("Translate", modelPosition, 0.01f, -5000, 5000, "%.3f");
      if (ImGui::Button("Close Me")) {
        showAnotherWindow = false;
      }
      ImGui::End();
    }

    Uint32 now = SDL_GetTicks();
    deltaTime = (now - lastFrame) / 1000.0f;
    lastFrame = now;
    ImGui::Render();
    glClearColor(.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gExampleShader.use();
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    gExampleShader.setMat4("view", view);
    gExampleShader.setMat4("projection", projection);
    gizmoShader.use();
    gizmoShader.setMat4("view", view);
    gizmoShader.setMat4("projection", projection);
    gizmoShader.setMat4("model", glm::translate(backpackOBJ.worldMatrix, glm::make_vec3(modelPosition)));

    backpackOBJ.draw(gExampleShader);
    backpackOBJ.drawGizmo(gizmoShader);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(gWindow);
    pollKeyboard();
    pollInputEvent();
  }
  return 0;
}

void pollKeyboard() {
  const Uint8* keystates = SDL_GetKeyboardState(NULL);
  if (keystates[SDL_SCANCODE_W]) {
    camera.ProcessKeyboard(xo::FORWARD, deltaTime);
  }
  if (keystates[SDL_SCANCODE_S]) {
    camera.ProcessKeyboard(xo::BACKWARD, deltaTime);
  }
  if (keystates[SDL_SCANCODE_A]) {
    camera.ProcessKeyboard(xo::LEFT, deltaTime);
  }
  if (keystates[SDL_SCANCODE_D]) {
    camera.ProcessKeyboard(xo::RIGHT, deltaTime);
  }
}
bool dragging = false;
int lastX, lastY;
void pollInputEvent() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    ImGui_ImplSDL2_ProcessEvent(&e);
    switch (e.type) {
      case SDL_QUIT:
        shouldRun = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_RIGHT) {
          dragging = true;
          SDL_GetRelativeMouseState(&lastX, &lastY);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (e.button.button == SDL_BUTTON_RIGHT) {
          dragging = false;
        }
      case SDL_MOUSEMOTION:
        if (dragging) {
          SDL_GetRelativeMouseState(&lastX, &lastY);
          camera.ProcessMouseMovement(lastX, lastY);
        }
        break;
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
            shouldRun = false;
            break;
        }
      case SDL_WINDOWEVENT:
        switch (e.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
            SCR_WIDTH = e.window.data1;
            SCR_HEIGHT = e.window.data2;
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        }
        break;
    }
  }
}