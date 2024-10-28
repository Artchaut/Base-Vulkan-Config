project "stb"
  kind "StaticLib"
  language "C++"
	architecture "x86_64"
  
  includedirs { "stb/" }

  files { "stb/*" }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
