/**
 * \file
 * \author Christien Alden (34119981)
*/
#pragma once
#include "AEngine/Events/ApplicationEvent.h"
#include "AEngine/Math/Math.h"
#include "Layer.h"
#include "Timer.h"
#include "Types.h"
#include <string>

#include "AEngine/ImGui/Editor.h"


extern int main(int, char**);

namespace AEngine
{
	class Window;

		/**
		 * \class Application
		 * \brief Encapsulates the application runtime
		*/
	class Application
	{
	public:
			/**
			 * \struct Properties
			 * \brief Holds the application properties
			*/
		struct Properties
		{
				/**
				 * \brief Application title which will be displayed in the title bar
				*/
			std::string title = "AEngine Demo";
				/**
				 * \brief Working directory of the application
				*/
			std::string workingDir;
		};

	public:
			/**
			 * \param[in] properties of the application
			*/
		explicit Application(const Properties& properties);
			/**
			 * \note Calls Shutdown()
			*/
		virtual ~Application();
			/**
			 * \brief Returns the instance of the application
			 * \return The instance of the application
			*/
		static Application& Instance();
			/**
			 * \brief Terminates the application runtime
			 * \note This is called internally when no windows are open
			*/
		void Terminate();
			/**
			 * \brief Pushes a layer onto the layer stack
			 * \param[in] layer The layer to push
			*/
		void SetLayer(UniquePtr<Layer> layer);
			/**
			 * \brief Returns the application window
			 * \return Window* to the application window
			*/
		Window* GetWindow() const;

		void EditMode(bool editMode);
		bool isEditMode() const;

	private:
			/**
			 * \brief Initialises the application specific modules.
			 * \note This is called in the constructor
			*/
		void Init();
			/**
			 * \brief Runs the application runtime
			*/
		void Run();
			/**
			 * \brief Shuts down the application specific modules
			 * \note This is called in the destructor
			*/
		void Shutdown();
			/**
			 * \brief Handles a window closed event
			 * \param[in] e The window closed event
			 * \retval true which stops other event handlers from processing the event
			*/
		bool OnWindowClose(WindowClosed& e);
			/**
			 * \brief Handles a window resized event
			 * \param[in] e The window resized event
			 * \retval false which enables other event handlers to process the event
			*/
		bool OnWindowResize(WindowResized& e);

	private:
		// core
		static Application* s_instance;
		Properties m_properties;
		UniquePtr<Window> m_window;

		// runtime
		bool m_minimised;
		bool m_running;
		UniquePtr<Layer> m_layer;
		Timer m_clock;
		bool m_editmode = false;

		friend int ::main(int, char**);

		Editor m_editor;
	};

	// define in client as this is called in EntryPoint.h
	Application* CreateApplication(Application::Properties& properties);
}
