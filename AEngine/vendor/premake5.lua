
project "glad"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	files {
		"glad/include/glad/glad.h",
		"glad/include/KHR/khrplatform.h",
		"glad/src/glad.c"
	}

	includedirs {
		"glad/include"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

--------------------------------------------------------------------------------
externalproject "glfw"
	os.execute("cmake -S glfw -B glfw")
	location "glfw/src"
	uuid (os.uuid("glfw"))
	kind "StaticLib"
