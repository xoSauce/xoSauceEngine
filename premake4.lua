#!lua

solution "XoSauce"
  
  local GLAD_OBJECT="deps/glad/glad.c"
  local IMGUI_SDL2_BACKEND = "deps/imgui/backends/imgui_impl_sdl.*"
  local IMGUI_OPENGL3_BACKEND = "deps/imgui/backends/imgui_impl_opengl3.*"
  local IMGUI_MAIN_CPP = "deps/imgui/*.cpp"
  local IMGUI_MAIN_H = "deps/imgui/*.h"
  libdirs {"lib/"}
  includedirs {
  "deps", 
  "deps/imgui",
  "src/Base"}
  buildoptions {"`sdl2-config --cflags `"}
  configurations {"Debug", "Release"}
  configuration "Debug"
    defines { "_DEBUG" }
    flags { "Symbols" }
    targetprefix "dbg_"

  configuration "Release"
    flags { "Optimize" }
  location "build"

  project "XoSauceEngine"
    kind "WindowedApp"
    language "C++"
    location "build"
    files {"src/**.cpp", "src/**.h", GLAD_OBJECT, IMGUI_SDL2_BACKEND, IMGUI_OPENGL3_BACKEND, IMGUI_MAIN_CPP, IMGUI_MAIN_H}
    links {"SDL2", "assimp", "dl", "IrrXML", "zlibstatic"}
  