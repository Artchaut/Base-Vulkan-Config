project "glm"
  kind "StaticLib"
  language "C++"
	architecture "x86_64"
  
  includedirs { "glm/" }

  files { "glm/glm/**" }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
