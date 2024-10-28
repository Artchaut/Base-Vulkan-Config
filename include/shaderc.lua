project "shaderc"
  kind "StaticLib"
  language "C++"
	architecture "x86_64"
  
  includedirs { "shaderc/", "libshaderc_util" }

  files { "shaderc/*.[h,hpp]", "libshaderc_util/*.h" }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
