workspace "BaseVulkanConfig"
  configurations { "Debug", "Release"}
  platforms { "Linux", "Win64" }

  startproject "BaseVulkanConfig"

  location "build"

  flags { "MultiProcessorCompile"}
  
  filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

project "BaseVulkanConfig"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	architecture "x86_64"
  
  location "build/"
  
  includedirs 
  { 
    "include/imgui/",
    "include/stb/",
    "include/tinyobjloader/",
    "include/shaderc/",

    "lib/glm/",
    "lib/glfw"
  }
  
  libdirs { os.findlib("vulkan", "shaderc_combined"),  "lib/glfw" }
 -- os.findlib("shaderc"),
  files { "src/*.cpp", "src/*.h"}
  
  links { "vulkan", "glfw", "glm", "imgui", "stb", "tinyobjloader", "shaderc_combined"}

  filter "system:linux"
    links { "dl", "pthread" }

include "include/imgui.lua"
include "include/stb.lua"
include "include/tinyobjloader.lua"
include "include/shaderc.lua"

include "lib/glfw.lua"
include "lib/glm.lua"
