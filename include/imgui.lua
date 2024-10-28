project "ImGui"
	kind "StaticLib"
	language "C++"
	architecture "x86_64"

	includedirs { "imgui/",  "glfw/include/" }

	files
	{
		"imgui/*.cpp",
	}

	defines 
	{
		"IMGUI_IMPL_VULKAN"
	}
    
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		defines
		{
			"_IMGUI_X11"
		}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		defines 
		{ 
			"_IMGUI_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
