/**
 * \file   CollisionBody.h
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#pragma once
#include "AEngine/Core/Types.h"
#include "AEngine/Math/Math.h"
#include "Collider.h"
#include <vector>

namespace AEngine
{
	class CollisionBody
	{
	public:
		virtual ~CollisionBody() = default;
			/**
			 * \brief Sets the transform (position and orientation) of the collision body.
			 *
			 * \param[in] position The position vector.
			 * \param[in] orientation The orientation quaternion.
			 */
		virtual void SetTransform(const Math::vec3& position, const Math::quat& orientation) = 0;
			/**
			 * \brief Gets the transform (position and orientation) of the collision body.
			 *
			 * \param[out] position The output parameter to store the position vector.
			 * \param[out] orientation The output parameter to store the orientation quaternion.
			 */
		virtual void GetTransform(Math::vec3& position, Math::quat& orientation) const = 0;
			/**
			 * \brief Adds a box collider to the collision body.
			 *
			 * \param[in] size The size of the box collider.
			 * \return Pointer to the created collider.
			 */
		virtual UniquePtr<Collider> AddBoxCollider(const Math::vec3& size, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) = 0;
		virtual UniquePtr<Collider> AddCapsuleCollider(float radius, float height, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) = 0;
		virtual UniquePtr<Collider> AddSphereCollider(float radius, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) = 0;
		virtual UniquePtr<Collider> GetCollider() = 0;
		virtual void RemoveCollider() = 0;
			/**
			 * \brief Gets the interpolated transform (position and orientation) of the collision body.
			 *
			 * \param[out] position The output parameter to store the interpolated position vector.
			 * \param[out] orientation The output parameter to store the interpolated orientation quaternion.
			 */
		virtual void GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation) = 0;
	};

	class RigidBody : public CollisionBody
	{
	public:
		enum class Type {
			STATIC,      ///< Static bodies are not affected by forces like gravity, and they cannot move.
			KINEMATIC,   ///< Kinematic bodies are special types of static bodies that can be moved by the user, but are not affected by forces like gravity.
			DYNAMIC      ///< Dynamic bodies are fully simulated. They can be moved manually by the user, but normally they are moved according to forces.
		};

	public:
		virtual ~RigidBody() = default;
			/**
			 * \brief Sets whether the rigid body has gravity.
			 *
			 * \param[in] hasGravity True to enable gravity, false otherwise.
			 */
		virtual void SetHasGravity(bool hasGravity) = 0;
			/**
			 * \brief Checks if the rigid body has gravity.
			 *
			 * \return True if the rigid body has gravity, false otherwise.
			 */
		virtual bool GetHasGravity() const = 0;
			/**
			 * \brief Sets the mass of the rigid body.
			 *
			 * \param[in] massKg The mass of the rigid body in kilograms.
			 */
		virtual void SetMass(float massKg) = 0;
			/**
			 * \brief Gets the mass of the rigid body.
			 *
			 * \return The mass of the rigid body in kilograms.
			 */
		virtual float GetMass() const = 0;
			/**
			 * \brief Sets the velocity of the rigid body.
			 *
			 * \param[in] velocity The velocity vector of the rigid body.
			 */
		virtual void SetVelocity(const Math::vec3& velocity) = 0;
			/**
			 * \brief Gets the velocity of the rigid body.
			 *
			 * \return The velocity vector of the rigid body.
			 */
		virtual const Math::vec3 GetVelocity() const = 0;
			/**
			 * \brief Sets the drag factor of the rigid body.
			 *
			 * \param[in] factor The drag factor of the rigid body.
			 */
		virtual void SetDrag(float factor) = 0;
			/**
			 * \brief Sets the type of the rigid body.
			 *
			 * \param[in] type The type of the rigid body.
			 */
		virtual void SetType(Type type) = 0;

		virtual Type GetType() const = 0;
	};
}
