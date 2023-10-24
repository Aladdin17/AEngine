/**
 * @file
 * @author Christien Alden (34119981)
**/
#pragma once
#include "AEngine/Core/PerspectiveCamera.h"
#include "AEngine/Core/TimeStep.h"

namespace AEngine
{
		/**
		 * @class DebugCamera
		 * @brief PerspectiveCamera used to explore the scene without interaction
		**/
	class DebugCamera : public PerspectiveCamera
	{
	public:
			/**
			 * \brief Default constructor
			*/
		DebugCamera() = default;
			/**
			 * \brief Constructor
			 * \param[in] fov Field of view
			 * \param[in] aspect Aspect ratio
			 * \param[in] nearPlane Near plane
			 * \param[in] farPlane Far plane
			*/
		DebugCamera(float fov, float aspect, float nearPlane, float farPlane);
			/**
			 * \brief Update the camera position, orientation and view matrix
			 * \param[in] frameTime this frame
			*/
		void OnUpdate(TimeStep frameTime);

			/**
			 * \brief Get the camera position
			 * \return Camera position
			*/
		Math::vec3 GetPosition() const;

			/**
			 * \brief Get the camera front vector
			 * \return Camera front vector
			*/
		Math::vec3 GetFront() const;
			/**
			 * \brief Get the camera yaw
			 * \return Camera yaw
			*/
		float GetYaw() const;
			/**
			 * \brief Get the camera pitch
			 * \return Camera pitch
			*/
		float GetPitch() const;
			/**
			 * \brief Get the camera movement speed
			 * \return Camera movement speed
			*/
		float GetMovementSpeed() const;
			/**
			 * \brief Get the camera look sensitivity
			 * \return Camera look sensitivity
			*/
		float GetLookSensitivity() const;

			/**
			 * \brief Set the camera position
			 * \param[in] pos New position
			*/
		void SetPosition(const Math::vec3& pos);
			/**
			 * \brief Set the camera yaw
			 * \param[in] yaw New yaw
			 * \note Values of \p yaw will be wrapped around to the range [0, 360]
			*/
		void SetYaw(float yaw);
			/**
			 * \brief Set the camera pitch
			 * \param[in] pitch New pitch
			 * \note Value of \p pitch will be clamped to the range [-89.0f, 89.0f]
			*/
		void SetPitch(float pitch);
			/**
			 * \brief Set the camera movement speed
			 * \param[in] step New movement speed
			 * \note Movement speed is in units/sec
			*/
		void SetMovementSpeed(float step);
			/**
			 * \brief Set the camera look sensitivity
			 * \param[in] sensitivity New look sensitivity
			*/
		void SetLookSensitivity(float sensitivity);

	private:
		Math::vec3 m_pos{ 0.0f, 0.0f, 0.0f };	///< Camera position
		Math::vec3 m_front{ 1.0f, 0.0f, 0.0f };	///< Camera front vector
		Math::vec3 m_up{ 0.0f, 1.0f, 0.0f };	///< Camera up vector
		Math::vec3 m_right{ 0.0f, 0.0f, 1.0f };	///< Camera right vector

		float m_moveSpeed{ 10.0f };				///< Translation speed in units/sec
		float m_lookSensitivity{ 5.0f };		///< Rotation sensitivity
		float m_pitch{ 0.0f };					///< Pitch offset
		float m_yaw{ 0.0f };					///< Yaw offset, face z-direction

			/**
			 * @brief Queries Input for cursor movement to update camera orientation
			**/
		void UpdateOrientation();
			/**
			 * @brief Queries Input for keypresses to update camera position
			**/
		void UpdatePosition(float dt);
			/**
			 * @brief Generates view matrix based on member variable values
			**/
		void GenerateViewMatrix();
	};
}
