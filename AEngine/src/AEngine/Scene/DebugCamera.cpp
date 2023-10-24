#include "DebugCamera.h"
#include "AEngine/Input/Input.h"

#include <algorithm>

namespace {
	static constexpr float internalLookModifier = 0.0025f;
}

namespace AEngine
{
	DebugCamera::DebugCamera(float fov, float aspect, float nearPlane, float farPlane)
		: PerspectiveCamera(fov, aspect, nearPlane, farPlane), m_moveSpeed{}, m_lookSensitivity{}, m_pitch{}, m_yaw{}
	{

	}

	void DebugCamera::OnUpdate(TimeStep frameTime)
	{
		UpdateOrientation();
		UpdatePosition(frameTime);
		GenerateViewMatrix();
	}

	Math::vec3 DebugCamera::GetPosition() const
	{
		return m_pos;
	}

	Math::vec3 DebugCamera::GetFront() const
	{
		return m_front;
	}

	float DebugCamera::GetYaw() const
	{
		return m_yaw;
	}

	float DebugCamera::GetPitch() const
	{
		return m_pitch;
	}

	float DebugCamera::GetMovementSpeed() const
	{
		return m_moveSpeed;
	}

	float DebugCamera::GetLookSensitivity() const
	{
		return m_lookSensitivity;
	}

	void DebugCamera::SetPosition(const Math::vec3& pos)
	{
		m_pos = pos;
	}

	void DebugCamera::SetYaw(float yaw)
	{
		float clampedYaw = std::fmod(yaw, 360.0f);
		if (clampedYaw < 0.0f)
		{
			clampedYaw += 360.0f;
		}
		else if (clampedYaw > 360.0f)
		{
			clampedYaw -= 360.0f;
		}

		m_yaw = clampedYaw;
	}

	void DebugCamera::SetPitch(float pitch)
	{
		m_pitch = std::clamp(pitch, -89.0f, 89.0f);
	}

	void DebugCamera::SetMovementSpeed(float speed)
	{
		m_moveSpeed = std::max(speed, 0.0f);
	}

	void DebugCamera::SetLookSensitivity(float sensitivity)
	{
		m_lookSensitivity = std::max(sensitivity, 0.0f);
	}

//--------------------------------------------------------------------------------
// Internal
//-------------------------------------------------------------------------------
	inline void DebugCamera::UpdateOrientation()
	{
		float lookStep = m_lookSensitivity * internalLookModifier;
		Math::vec2 offset = Input::GetMouseDelta();
		// update pitch / yaw
		m_pitch -=  lookStep * offset.y;
		m_yaw += lookStep * offset.x;

		// clamp pitch
		m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

		// clamp yaw
		if (m_yaw >= 360.0f)
		{
			m_yaw = 0.0f;
		}
		else if (m_yaw <= -360.0f)
		{
			m_yaw = 0.0f;
		}

		// update member variables
		Math::vec3 front;
		float pitchRad = Math::radians(m_pitch);
		float yawRad = Math::radians(m_yaw);
		front.x = Math::cos(yawRad) * Math::cos(pitchRad);
		front.y = Math::sin(pitchRad);
		front.z = Math::sin(yawRad) * Math::cos(pitchRad);
		m_right = Math::normalize(Math::cross(front, m_up));
		m_front = Math::normalize(front);
	}

	inline void DebugCamera::UpdatePosition(float dt)
	{
		float movementStep = m_moveSpeed * dt;

		// forward/back
		if (Input::IsKeyPressed(AEKey::W))
			m_pos += movementStep * m_front;
		if (Input::IsKeyPressed(AEKey::S))
			m_pos -= movementStep * m_front;

		// strafe
		if (Input::IsKeyPressed(AEKey::A))
			m_pos -= movementStep * m_right;
		if (Input::IsKeyPressed(AEKey::D))
			m_pos += movementStep * m_right;

		// up/down
		if (Input::IsKeyPressed(AEKey::SPACE))
			m_pos += movementStep * m_up;
		if (Input::IsKeyPressed(AEKey::LEFT_SHIFT))
			m_pos -= movementStep * m_up;
	}

	inline void DebugCamera::GenerateViewMatrix()
	{
		m_view = Math::lookAt(m_pos, m_pos + m_front, m_up);
	}
}
