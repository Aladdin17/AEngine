/**
 * \file
 * \author Christien Alden (34119981)
*/
#pragma once
#include "AEngine/Math/Math.h"
#include "AEngine/Events/EventHandler.h"
#include "AEngine/Input/InputBuffer.h"
#include "Types.h"
#include <optional>
#include <string>

namespace AEngine
{
		/**
		 * \enum WindowAPI
		 * \brief AEngine supported windowing APIs
		*/
	enum class WindowAPI
	{
		NONE = 0, GLFW
	};

		/**
		 * \class Window
		 * \brief Window Interface
		*/
	class Window
	{
	public:
			/**
			 * \struct Properties
			 * \brief Holds properties of window
			*/
		struct Properties
		{
			Properties() = default;
			Properties(const std::string& title, unsigned int width, unsigned int height, bool visible = true)
				: title{ title }, width{ width }, height{ height }, visible{ visible } {}

			bool visible{ true };
			std::string title{ "AEngine Window" };
			unsigned int width{ 1600 };
			unsigned int height{ 900 };
		};

	public:
			/**
			 * \brief Destructor
			*/
		virtual ~Window() = default;
			/**
			 * \brief Returns windowing API's native window
			 * \return void* to native window
			*/
		virtual void* GetNative() const = 0;

			/**
			 * \brief Makes this window the current render target
			*/
		void MakeCurrent() const;
			/**
			 * \brief Swaps the buffers for this window
			*/
		void SwapBuffers() const;
			/**
			 * \brief Sets the cursor state for this window
			 * \param[in] toggle if true, cursor will be visible, if false, cursor will be hidden, defaults to true
			*/
		void ShowCursor(bool toggle = true) const;
			/**
			 * \brief Returns if cursor is showing
			 * \retval True if cursor is showing
			 * \retval False if cursor is not showing
			*/
		bool IsShowingCursor() const;

			/**
			 * \brief Returns title of window
			 * \return The title of the window
			*/
		const std::string& GetTitle() const;
			/**
			 * \brief Returns size of window
			 * \return The size of the window
			 * \details
			 * The size is returned as a Math::uvec2
			 * - x = width
			 * - y = height
			*/
		Math::uvec2 GetSize() const;

			/**
			 * \brief Maximises window
			*/
		virtual void Maximise() = 0;
			/**
			 * \brief Minimises window into taskbar
			*/
		virtual void Minimise() = 0;
			/**
			 * \brief Returns if window is visible
			 * \retval True if window is visible
			 * \retval False if window is not visible
			*/
		virtual bool IsVisible() const;

			/**
			 * \brief Sets if window is visible
			 * \param[in] visible if true, window will be visible, if false, window will be hidden
			*/
		virtual void SetVisible(bool visible) = 0;
			/**
			 * \brief Sets position of window
			 * \param[in] xpos new x position of window, if not specified, x position will not be changed
			 * \param[in] ypos new y position of window, if not specified, y position will not be changed
			 * \note Pass in std::nullopt use existing value
			*/
		virtual void SetPosition(std::optional<unsigned int> xpos, std::optional<unsigned int> ypos) const = 0;
			/**
			 * \brief Returns position of window
			 * \return The position of the window
			 * \details
			 * The position is returned as a Math::uvec2
			 * - x = x position
			 * - y = y position
			*/
		virtual Math::uvec2 GetPosition() const = 0;

			/**
			 * \brief Sets title of window
			 * \param[in] title new title of window
			*/
		virtual void SetTitle(const std::string& title) = 0;
			/**
			 * \brief Sets size of window
			 * \param[in] width new width of window, if not specified, width will not be changed
			 * \param[in] height new height of window, if not specified, height will not be changed
			 * \note Pass in std::nullopt use existing value
			*/
		virtual void SetSize(std::optional<unsigned int> width, std::optional<unsigned int> height) = 0;

			/**
			 * \brief Register a handler for an event
			 * \tparam T Type of event to register handler for
			 * \param priority Priority of handler with 0 as the highest priority
			 * \param handler Handler function
			 * \note Handler function should return true if event was handled and should not be passed to other handlers
			 * \detailsn
			 * The handlers will be called in ascending order of priority, but
			 * the order of handlers with the same priority is not guaranteed.
			*/
		template <typename T>
		void RegisterEventHandler(int layer, const std::function<bool(T&)>& handler)
		{
			m_eventHandler.RegisterHandler<T>(layer, handler);
		}

		/// \remark There has got to be a better way to do this...
		void PostEvent(UniquePtr<Event> event);
		void SetKeyState(AEKey key, AEInputState state);
		void SetMouseButtonState(AEMouse button, AEInputState state);
		void SetMousePosition(const Math::vec2& position);
		void SetMouseScroll(const Math::vec2& scroll);

	protected:
			/**
			 * \brief Constructor
			 * \param[in] properties initial properties of window
			*/
		Window(Properties properties);
			/**
			 * \brief Properties of window
			*/
		Properties m_properties;
		EventHandler m_eventHandler;


		// new input states
		std::array<AEInputState, static_cast<Size_t>(AEKey::INVALID)> m_keyState{};
		std::array<AEInputState, static_cast<Size_t>(AEMouse::INVALID)> m_mouseState{};
		Math::vec2 m_mousePosition;
		Math::vec2 m_mouseScroll;

		// old input states
		std::array<AEInputState, static_cast<Size_t>(AEKey::INVALID)> m_keyStateLast{};
		std::array<AEInputState, static_cast<Size_t>(AEMouse::INVALID)> m_mouseStateLast{};
		Math::vec2 m_mousePositionLast;
		Math::vec2 m_mouseScrollLast;

			/**
			 * \brief Runtime update of window
			 * \details
			 * Polls for events and swaps buffers
			*/
		virtual void OnUpdate() = 0;
		void SwapInputBuffers();
		void OnUpdateInput();

			/**
			 * \brief Creates a new window
			 * \param[in] properties initial properties of window
			 * \return A UniquePtr to the new window
			*/
		static UniquePtr<Window> Create(const Properties& properties = Properties());

		friend class Application;
	};
}
