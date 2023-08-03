call vendor\cmake\bin\cmake.exe -S . -B build -G "Visual Studio 17 2022"
@echo off
if not "%1" == "0" (
	PAUSE
)
