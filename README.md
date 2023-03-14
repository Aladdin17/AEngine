# ACHO Interactive

- Christien Alden
- Geoff Candy
- Ben Hawkins
- Lane O'Rafferty


# How to build

1. When first checking out branch, the submodules will need to be initialised
	- Run `git submodule update --init`
	- This will clone remote repositories into the correct place
1. If the project has not been generated, run *GenerateProjects.bat* to generate a solution for Visual Studio 2022
1. Navigate to *ACHO Engine Development.sln* in the `build` directory
1. Choose either the **Debug** or **Release** configuration to build
