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
			 * \param[in] position The position vector.
			 * \param[in] orientation The orientation quaternion.
			 */
		virtual void SetTransform(const Math::vec3& position, const Math::quat& orientation) = 0;
			/**
			 * \brief Gets the transform (position and orientation) of the collision body.
			 * \param[out] position The output parameter to store the position vector.
			 * \param[out] orientation The output parameter to store the orientation quaternion.
			 */
		virtual void GetTransform(Math::vec3& position, Math::quat& orientation) const = 0;

			/**
			 * \brief Adds a box collider to the collision body.
			 * \param[in] size The size of the box collider.
			 * \return Pointer to the created collider.
			 */
		virtual UniquePtr<Collider> AddBoxCollider(const Math::vec3& size, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) = 0;
			/**
			 * \brief Adds a sphere collider to the collision body.
			 * \param[in] radius The radius of the sphere collider.
			 * \return Pointer to the created collider.
			*/
		virtual UniquePtr<Collider> AddCapsuleCollider(float radius, float height, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) = 0;
			/**
			 * \brief Adds a sphere collider to the collision body.
			 * \param[in] radius The radius of the sphere collider.
			 * \return Pointer to the created collider.
			*/
		virtual UniquePtr<Collider> AddSphereCollider(float radius, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) = 0;
			/**
			 * \brief Gets the collider of the collision body.
			 * \return Pointer to the collider.
			*/
		virtual UniquePtr<Collider> GetCollider() = 0;
			/**
			 * \brief Removes the collider from the collision body.
			*/
		virtual void RemoveCollider() = 0;

			/**
			 * \brief Gets the interpolated transform (position and orientation) of the collision body.
			 * \param[out] position The output parameter to store the interpolated position vector.
			 * \param[out] orientation The output parameter to store the interpolated orientation quaternion.
			 */
		virtual void GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation) = 0;
	};

	class RigidBody : public CollisionBody
	{
	public:
		enum class Type {
			Static,      ///< Static bodies are not affected by forces like gravity, and they cannot move.
			Kinematic,   ///< Kinematic bodies are special types of static bodies that can be moved by the user, but are not affected by forces like gravity.
			Dynamic      ///< Dynamic bodies are fully simulated. They can be moved manually by the user, but normally they are moved according to forces.
		};

	public:
		virtual ~RigidBody() = default;

			/**
			 * \brief Sets the type of the rigid body.
			 * \param[in] type The type of the rigid body.
			 */
		virtual void SetType(Type type) = 0;
			/**
			 * \brief Returns the type of the rigid body.
			*/
		virtual Type GetType() const = 0;

			/**
			 * \brief Sets the mass of the rigid body.
			 * \param massKg The mass of the rigid body in kilograms.
			 */
		virtual void SetMass(float massKg) = 0;
			/**
			 * \brief Returns the mass of the rigid body.
			 * \return The mass of the rigid body in kilograms.
			 */
		virtual float GetMass() const = 0;
			/**
			 * \brief Sets the inertia tensor for the rigid body.
			 * \param[in] inertiaTensor The inertia tensor for the rigid body.
			*/
		virtual void SetInertiaTensor(const Math::mat3& inertiaTensor) = 0;
			/**
			 * \brief Returns the inertia tensor of the rigid body.
			*/
		virtual Math::mat3 GetInertiaTensor() const = 0;
			/**
			 * \brief Sets whether the rigid body has gravity or not.
			 * \param hasGravity Specifies if the rigid body has gravity.
			 */
		virtual void SetHasGravity(bool hasGravity) = 0;
			/**
			 * \brief Returns whether the rigid body has gravity or not.
			 *
			 * \return True if the rigid body has gravity, false otherwise.
			 */
		virtual bool GetHasGravity() const = 0;

			/**
			 * \brief Sets the linear damping coefficient of the rigid body.
			 * \param[in] damping The linear damping coefficient of the rigid body.
			 * \note
			 * The \p damping value should be between 0 and 1. If the damping
			 * value is 0, no damping is applied to the rigid body. If it is 1, then
			 * the rigid body velocity will be set to zero after the timestep.\n
			 * Values outside of this range will be clamped.
			*/
		virtual void SetLinearDamping(float damping) = 0;
			/**
			 * \brief Returns the linear damping coefficient of the rigid body.
			 * \return The linear damping coefficient of the rigid body.
			*/
		virtual float GetLinearDamping() const = 0;
			/**
			 * \brief Sets the angular damping coefficient of the rigid body.
			 * \param[in] damping The angular damping coefficient of the rigid body.
			*/
		virtual void SetAngularDamping(float damping) = 0;
			/**
			 * \brief Returns the angular damping coefficient of the rigid body.
			 * \return The angular damping coefficient of the rigid body.
			*/
		virtual float GetAngularDamping() const = 0;


			/**
			 * \brief Sets the velocity of the rigid body.
			 * \param velocity The new velocity of the rigid body.
			 */
		virtual void SetLinearVelocity(const Math::vec3& velocity) = 0;
			/**
			 * \brief Returns the velocity of the rigid body.
			 * \return The velocity of the rigid body.
			 */
		virtual const Math::vec3 GetLinearVelocity() const = 0;
			/**
			 * \brief Sets the angular velocity of the rigid body.
			 * \param velocity The new angular velocity of the rigid body.
			*/
		virtual void SetAngularVelocity(const Math::vec3& velocity) = 0;
			/**
			 * \brief Returns the angular velocity of the rigid body.
			 * \return The angular velocity of the rigid body.
			*/
		virtual const Math::vec3 GetAngularVelocity() const = 0;
	};
}
