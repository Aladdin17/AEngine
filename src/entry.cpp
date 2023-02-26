/**
 * @file
 * @author Christien Alden (34119981)
 * @brief Provides an entry point for entire project
**/
#include "AEngine/AEngine.h"

int main(int argc, char** argv)
{
	// initialise program
	AEngine::Logger::Init();
	AE_LOG_INFO("Runtime::Initialising");
	std::unique_ptr<AEngine::Window> window =
		AEngine::Window::Create({ "AEngine Demo", 1600, 900 });

	// load shaders
	AEngine::ShaderManager::Instance()->LoadShader(
		"flat", "resources/shaders/flat_vertex.shader", "resources/shaders/flat_fragment.shader");
	AEngine::ShaderManager::Instance()->LoadShader(
		"diffuse", "resources/shaders/diffuse_vertex.shader", "resources/shaders/diffuse_fragment.shader");
	AEngine::ShaderManager::Instance()->LoadShader(
		"font", "resources/shaders/font_vertex.shader", "resources/shaders/font_fragment.shader");

	glfwSetKeyCallback(static_cast<GLFWwindow*>(window->GetNative()), AEngine::InputManager::KeyCallback);

	// run scenes
	AE_LOG_INFO("Runtime::MainLoop");
	while (!glfwWindowShouldClose(static_cast<GLFWwindow*>(window->GetNative())))
	{
		if (AEngine::InputManager::IsKeyNoRepeat(AE_KEY_ESCAPE))
			glfwSetWindowShouldClose(static_cast<GLFWwindow*>(window->GetNative()), true);

		window->OnUpdate();
	}

	// teardown
	AE_LOG_INFO("Runtime::Terminate");
	AEngine::ShaderManager::Instance()->Clear();

	return 0;
}
