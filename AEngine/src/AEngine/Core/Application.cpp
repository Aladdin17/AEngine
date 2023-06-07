/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "Application.h"
#include "AEngine/Core/Logger.h"
#include "AEngine/Events/ApplicationEvent.h"
#include "AEngine/Events/EventQueue.h"
#include "AEngine/Render/RenderCommand.h"
#include "AEngine/Resource/AssetManager.h"
#include "AEngine/Scene/SceneManager.h"
#include "TimeStep.h"
#include "Window.h"

namespace AEngine
{
	// forward declarations
	class Model;
	class Script;
	class Shader;
	class Texture;

	// static initialisation
	Application *Application::s_instance = nullptr;

	Application::Application(const Properties& properties)
		: m_properties{ properties },
		  m_window{ nullptr }, m_running{ true },
		  m_minimised{ false }, m_layers{},
		  m_clock{}
	{
		// if applicaton already created
		if (s_instance)
		{
			AE_LOG_FATAL("Application::Constructor::Fatal -> Application can not have multiple instances");
		}

		// set instance and initialise
		s_instance = this;
		Init();
	}

	Application::~Application()
	{
		AE_LOG_INFO("Application::Destructor");
		Shutdown();
	}

	Application& Application::Instance()
	{
		if (s_instance)
		{
			return *s_instance;
		}

		AE_LOG_FATAL("Application::Instance::Fatal -> None present");
	}

	void Application::Terminate()
	{
		m_running = false;
	}

	void Application::PushLayer(UniquePtr<Layer> layer)
	{
		m_layers.PushLayer(std::move(layer));
	}

	Window* Application::GetWindow() const
	{
		return m_window.get();
	}

	void Application::Init()
	{
		AE_LOG_INFO("Application::Init");
		RenderCommand::Initialise(RenderLibrary::OpenGL);
		m_window = AEngine::Window::Create({ m_properties.title, 1600, 900 });

		// setup default render state
		RenderCommand::SetClearColor(Math::vec4{ 255.0f, 255.0f, 255.0f, 255.0f });
		RenderCommand::EnableDepthTest(true);

		// set default cursor state
		m_window->ShowCursor(true);
	}

	bool Application::OnWindowClose(WindowClosed& e)
	{
		Terminate();
		return true;
	}

	bool Application::OnWindowResize(WindowResized& e)
	{
		unsigned int width = e.GetWidth();
		unsigned int height = e.GetHeight();
		m_minimised = (width == 0 && height == 0) ? true : false;
		RenderCommand::SetViewport(0, 0, width, height);

		/// \todo Find a better solution than this for handling resize events
		const std::vector<std::string> scenes = SceneManager::GetSceneIdents();
		for (const std::string& ident : scenes)
		{
			SceneManager::GetScene(ident)->OnViewportResize(width, height);
		}

		return false;
	}

	void Application::Shutdown()
	{
		AE_LOG_INFO("Applicaton::Shutdown");
		m_layers.Clear();
		AEngine::AssetManager<Model>::Instance().Clear();
		AEngine::AssetManager<Script>::Instance().Clear();
		AEngine::AssetManager<Shader>::Instance().Clear();
		AEngine::AssetManager<Texture>::Instance().Clear();
	}

	// must be called externally
	void Application::Run()
	{
		AE_LOG_INFO("Application::Run");

		m_clock.Start();
		while (m_running)
		{
			TimeStep dt = m_clock.GetDelta();

			// poll for application events
			EventDispatcher e;
			e.Dispatch<WindowClosed>(AE_EVENT_FN(&Application::OnWindowClose));
			e.Dispatch<WindowResized>(AE_EVENT_FN(&Application::OnWindowResize));

			// if the window is minimised, don't update the layers
			// the engine will still poll input and swap the buffers
			if (!m_minimised)
			{
				// update layers
				for(auto it = m_layers.begin(); it != m_layers.end(); ++it)
				{
					(*it)->OnUpdate(dt);
				}
			}

			m_window->OnUpdate();
		}
	}
}
