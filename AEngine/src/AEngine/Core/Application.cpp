/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "AEngine/Core/Logger.h"
#include "AEngine/Events/EventHandler.h"
#include "AEngine/Input/InputBuffer.h"
#include "AEngine/Render/RenderCommand.h"
#include "AEngine/Render/ResourceAPI.h"
#include "AEngine/Resource/AssetManager.h"
#include "AEngine/Scene/SceneManager.h"
#include "AEngine/Render/RenderPipeline.h"
#include "Application.h"
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
		  m_minimised{ false }, m_layer{},
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

	void Application::SetLayer(UniquePtr<Layer> layer)
	{
		if (m_layer)
		{
			m_layer->OnDetach();
		}

		m_layer = std::move(layer);
		m_layer->OnAttach();
	}

	Window* Application::GetWindow() const
	{
		return m_window.get();
	}

	void Application::Init()
	{
		AE_LOG_INFO("Application::Init");
		ResourceAPI::Initialise(ModelLoaderLibrary::Assimp);
		RenderCommand::Initialise(RenderLibrary::OpenGL);
		m_window = AEngine::Window::Create({ m_properties.title, 1600, 900 });
		EditorProperties props;
		m_editor.Init(m_window.get(), props);

		// setup application event callbacks
		// using priority level 0 to give application layer priority
		m_window->RegisterEventHandler<WindowClosed>(0, AE_EVENT_FN(&Application::OnWindowClose));
		m_window->RegisterEventHandler<WindowResized>(0, AE_EVENT_FN(&Application::OnWindowResize));

		// setup input callbacks
		// using priority level 1 to give gui layer priority
		m_window->RegisterEventHandler<KeyPressed>(1, [](KeyPressed& e) -> bool {
			InputBuffer::Instance().SetKeyState(e.GetKey(), true);
			return true;
		});
		m_window->RegisterEventHandler<KeyReleased>(1, [](KeyReleased& e) -> bool {
			InputBuffer::Instance().SetKeyState(e.GetKey(), false);
			return true;
		});
		m_window->RegisterEventHandler<MouseMoved>(1, [](MouseMoved& e) -> bool {
			InputBuffer::Instance().SetMousePosition(e.GetPos());
			return true;
		});
		m_window->RegisterEventHandler<MouseButtonPressed>(1, [](MouseButtonPressed& e) -> bool {
			InputBuffer::Instance().SetMouseButtonState(e.GetButton(), true);
			return true;
		});
		m_window->RegisterEventHandler<MouseButtonReleased>(1, [](MouseButtonReleased& e) -> bool {
			InputBuffer::Instance().SetMouseButtonState(e.GetButton(), false);
			return true;
		});
		m_window->RegisterEventHandler<MouseScrolled>(1, [](MouseScrolled& e) -> bool {
			InputBuffer::Instance().SetMouseScroll(e.GetScroll());
			return true;
		});

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

		RenderPipeline::Instance().OnWindowResize({e.GetWidth(), e.GetHeight()});

		return false;
	}

	void Application::Shutdown()
	{
		m_editor.Shutdown();
		AE_LOG_INFO("Applicaton::Shutdown");
		m_layer->OnDetach();
		SceneManager().UnloadAllScenes();
		AEngine::AssetManager<Model>::Instance().Clear();
		AEngine::AssetManager<Script>::Instance().Clear();
		AEngine::AssetManager<Shader>::Instance().Clear();
		AEngine::AssetManager<Texture>::Instance().Clear();
	}

	void Application::EditMode(bool editMode)
	{
		m_editmode = editMode;
	}

	bool Application::isEditMode() const
	{
		return m_editmode;
	}

	// must be called externally
	void Application::Run()
	{
		AE_LOG_INFO("Application::Run");
		
		m_clock.Start();
		while (m_running)
		{
			TimeStep dt = m_clock.GetDelta();
			
			m_editor.CreateNewFrame();
			// if the window is minimised, don't update the layers
			// the engine will still poll input and swap the buffers
			if (!m_minimised)
			{
				m_layer->OnUpdate(dt);
			}
			
			// update the editor
			m_editor.Update();
			m_editor.Render();

			// update input and swap buffers
			InputBuffer::Instance().OnUpdate();
			m_window->OnUpdate();
		}
	}
}
