--[[]
	author: Christien Alden (34119981)
	purpose: Build script for ACHO Engine workspace
--]]

-- directories
exeDir = "executable/"
outputDir = "%{cfg.buildcfg}/"
projDir = "%{prj.name}/"
engineDir = "%{_MAIN_SCRIPT_DIR}/" .. "AEngine/"

----------------------------------------------------------------------------------------------------
workspace "ACHO Engine Development"
	location "build"
	architecture "x64"
	startproject "AEngine-Editor"
	targetdir (exeDir .. outputDir .. projDir)
	objdir ("%{cfg.location}/" .. outputDir .. projDir)

	configurations {
		"Debug",
		"Release"
	}

	group "Core"
		include "AEngine"
		include "AEngine-Editor"
	group ""

	group "Dependencies"
		include "AEngine/dependencies.lua"
	group ""
