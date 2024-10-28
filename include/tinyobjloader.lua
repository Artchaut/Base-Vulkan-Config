project "tinyobjloader"
  kind "StaticLib"
  language "C++"
	architecture "x86_64"
  
  includedirs { "tinyobjloader/" }

  files { "tinyobjloader/*" }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
