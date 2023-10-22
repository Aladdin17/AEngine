#include "InputBuffer.h"
#include "AEngine/Core/Application.h"

namespace AEngine
{
//--------------------------------------------------------------------------------
// Frontend functions -> exposed by Input.h
//--------------------------------------------------------------------------------
	bool InputBuffer::IsKeyPressed(AEKey key)
	{
		return m_keyState[static_cast<Size_t>(key)];
	}

	bool InputBuffer::IsKeyPressedNoRepeat(AEKey key)
	{
		return IsKeyPressed(key) && !m_keyStateLast[static_cast<Size_t>(key)];
	}

	bool InputBuffer::IsMouseButtonPressed(AEMouse button)
	{
		return m_mouseButtonState[static_cast<Size_t>(button)];
	}

	bool InputBuffer::IsMouseButtonPressedNoRepeat(AEMouse button)
	{
		return IsMouseButtonPressed(button) && !m_mouseButtonStateLast[static_cast<Size_t>(button)];
	}

	Math::vec2 InputBuffer::GetMousePosition()
	{
		return m_mousePosition;
	}

	Math::vec2 InputBuffer::GetMouseDelta()
	{
		if(Application::Instance().isEditMode())
			return glm::vec2(0.0f, 0.0f);
		else
			return m_mousePosition - m_mousePositionLast;
	}

	Math::vec2 InputBuffer::GetMouseScroll()
	{
		return m_mouseScroll;
	}

//--------------------------------------------------------------------------------
// Backend functions
//--------------------------------------------------------------------------------
	InputBuffer& InputBuffer::Instance()
	{
		static InputBuffer instance;
		return instance;
	}

	void InputBuffer::OnUpdate()
	{
		for (Size_t i = 0; i < static_cast<Size_t>(AEKey::INVALID); i++)
		{
			m_keyStateLast[i] = m_keyState[i];
		}

		for (Size_t i = 0; i < static_cast<Size_t>(AEMouse::INVALID); i++)
		{
			m_mouseButtonStateLast[i] = m_mouseButtonState[i];
		}

		m_mousePositionLast = m_mousePosition;
		m_mouseScroll = Math::vec2{ 0.0f, 0.0f };
	}

	void InputBuffer::SetKeyState(AEKey key, bool state)
	{
		m_keyState[static_cast<Size_t>(key)] = state;
	}

	void InputBuffer::SetMouseButtonState(AEMouse button, bool state)
	{
		m_mouseButtonState[static_cast<Size_t>(button)] = state;
	}

	void InputBuffer::SetMousePosition(const Math::vec2& position)
	{
		m_mousePosition = position;
	}

	void InputBuffer::SetMouseScroll(const Math::vec2& scroll)
	{
		m_mouseScroll = scroll;
	}
}
