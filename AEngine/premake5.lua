--[[]
	author: Christien Alden (34119981)
	purpose: Build script for AEngine project
--]]

project "AEngine"
	kind "StaticLib"
	staticruntime "Off"
	language "C++"

	defines {
	  "GLFW_INCLUDE_NONE"
	}

	files {
		"include/**",
		"src/**"
	}

	links {
		"opengl32.lib",
		"glfw",
		"glad"
	}

	includedirs {
		"src",  -- internal source files
		"include",  -- external header files
		"vendor/entt/include",
		"vendor/glad/include",
		"vendor/glfw/include",
		"vendor/glm/include",
		"vendor/spdlog/include",
		"vendor/stb/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		warnings "Extra"
		disablewarnings {
			"4100",
			"4505"
		}
		-- disable warnings on vendor files
		externalincludedirs "vendor/**"
		externalwarnings "Default"

	filter "configurations:Debug"
		runtime "Debug"
		defines {"DEBUG", "AE_DEBUG"}
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "DNDEBUG"
		optimize "On"
