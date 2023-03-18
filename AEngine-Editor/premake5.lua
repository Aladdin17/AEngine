project "AEngine-Editor"
	kind "ConsoleApp"
	language "C++"
	debugdir "."

	files {
		"src/**"
	}

	includedirs {
		engineDir .. "include",
		-- todo: remove me!
		engineDir .. "vendor"
	}

	links {
		"AEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		warnings "Extra"
		-- todo: remove me!
		externalincludedirs (engineDir .. "vendor/**")
		externalwarnings "Default"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines {
			"DEBUG",
			"AE_DEBUG"
		}

	filter "configurations:Release"
		runtime "Release"
		defines "DNDEBUG"
		optimize "On"
