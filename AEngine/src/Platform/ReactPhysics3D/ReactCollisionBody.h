/**
 * \file   ReactCollisionBody.h
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include "AEngine/Math/Math.h"
#include "AEngine/Physics/Collider.h"
#include "AEngine/Physics/CollisionBody.h"
#include "Platform/ReactPhysics3D/ReactPhysics.h"

namespace AEngine
{
		/**
		 * \class ReactCollisionBody
		 * \brief Represents a collision body using the ReactPhysics3D library.
		 *
		 * This class inherits from CollisionBody.
		 */
	class ReactCollisionBody : public CollisionBody
	{
	public:
			/**
			 * \brief Constructs a ReactCollisionBody object with the given parameters.
			 * \param[in] world The ReactPhysicsWorld associated with the collision body.
			 * \param[in] position The initial position of the collision body.
			 * \param[in] orientation The initial orientation of the collision body.
			 */
		ReactCollisionBody(ReactPhysicsWorld* world, const Math::vec3& position, const Math::quat& orientation);
		virtual ~ReactCollisionBody();

			/**
			 * \copydoc CollisionBody::SetTransform
			 */
		virtual void SetTransform(const Math::vec3&, const Math::quat&) override;
			/**
			 * \copydoc CollisionBody::GetTransform
			 */
		virtual void GetTransform(Math::vec3&, Math::quat&) const override;
			/**
			 * \copydoc CollisionBody::AddBoxCollider
			 */
		virtual UniquePtr<Collider> AddBoxCollider(const Math::vec3& size, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \copydoc CollisionBody::AddCapsuleCollider
			*/
		virtual UniquePtr<Collider> AddCapsuleCollider(float radius, float height, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \copydoc CollisionBody::AddSphereCollider
			*/
		virtual UniquePtr<Collider> AddSphereCollider(float radius, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \copydoc CollisionBody::GetCollider
			*/
		virtual UniquePtr<Collider> GetCollider() override;
			/**
			 * \copydoc CollisionBody::RemoveCollider
			*/
		virtual void RemoveCollider() override;

			/**
			 * \copydoc CollisionBody::GetCollider
			 */
		virtual void GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation) override;

			/**
			 * \brief Returns the native collision body object.
			 * \return A pointer to the native ReactPhysics3D CollisionBody object.
			 */
		rp3d::CollisionBody* GetNative() const;

	protected:
		rp3d::CollisionBody* m_body;		///< The native ReactPhysics3D CollisionBody object.
		ReactPhysicsWorld* m_world;			///< The ReactPhysicsWorld associated with the collision body.
		rp3d::Transform m_lastTransform;	///< The last transform of the collision body.
	};

		/**
		 * \class ReactRigidBody
		 * \brief Represents a rigid body using the ReactPhysics3D library.
		 *
		 * This class inherits from RigidBody.
		 */
	class ReactRigidBody : public RigidBody
	{
	public:
			/**
			 * \brief Constructs a ReactRigidBody object with the given parameters.
			 * \param world The ReactPhysicsWorld associated with the rigid body.
			 * \param position The initial position of the rigid body.
			 * \param orientation The initial orientation of the rigid body.
			 */
		ReactRigidBody(ReactPhysicsWorld* world, const Math::vec3& position, const Math::quat& orientation);
		virtual ~ReactRigidBody() = default;

			/**
			 * \copydoc RigidBody::SetType
			 */
		virtual void SetType(Type type) override;
			/**
			 * \copydoc RigidBody::GetType
			*/
		virtual Type GetType() const override;

			/**
			 * \copydoc RigidBody::SetMass
			 */
		virtual void SetMass(float massKg) override;
			/**
			 * \copydoc RigidBody::GetMass
			 */
		virtual float GetMass() const override;
			/**
			 * \copydoc RigidBody::SetLinearDamping
			*/
		virtual void SetLinearDamping(float damping) override;
			/**
			 * \copydoc RigidBody::GetLinearDamping
			*/
		virtual float GetLinearDamping() const override;
			/**
			 * \copydoc RigidBody::SetHasGravity
			 */
		virtual void SetHasGravity(bool hasGravity) override;
			/**
			 * \copydoc RigidBody::GetHasGravity
			 */
		virtual bool GetHasGravity() const override;

			/**
			 * \copydoc RigidBody::SetLinearAcceleration
			*/
		virtual void SetLinearAcceleration(Math::vec3 acceleration) override;
			/**
			 * \copydoc RigidBody::GetLinearAcceleration
			*/
		virtual Math::vec3 GetLinearAcceleration() const override;
			/**
			 * \copydoc RigidBody::SetAngularAcceleration
			*/
		virtual void SetAngularAcceleration(Math::vec3 acceleration) override;
			/**
			 * \copydoc RigidBody::GetAngularAcceleration
			*/
		virtual Math::vec3 GetAngularAcceleration() const override;
			/**
			 * \copydoc RigidBody::SetLinearVelocity
			 */
		virtual void SetLinearVelocity(const Math::vec3& velocity) override;
			/**
			 * \copydoc RigidBody::GetLinearVelocity
			 */
		virtual const Math::vec3 GetLinearVelocity() const override;
			/**
			 * \copydoc RigidBody::SetAngularVelocity
			*/
		virtual void SetAngularVelocity(const Math::vec3& velocity) override;
			/**
			 * \copydoc RigidBody::GetAngularVelocity
			*/
		virtual const Math::vec3 GetAngularVelocity() const override;


		//--------------------------------------------------------------------------------
		// From CollisionBody
		//--------------------------------------------------------------------------------
			/**
			 * \copydoc ReactCollisionBody::SetTransform
			 */
		virtual void SetTransform(const Math::vec3& position, const Math::quat& orientation) override;
			/**
			 * \copydoc ReactCollisionBody::GetTransform
			 */
		virtual void GetTransform(Math::vec3& position, Math::quat& orientation) const override;
			/**
			 * \copydoc ReactCollisionBody::AddBoxCollider
			 */
		virtual UniquePtr<Collider> AddBoxCollider(const Math::vec3& size, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \copydoc ReactCollisionBody::AddCapsuleCollider
			*/
		virtual UniquePtr<Collider> AddCapsuleCollider(float radius, float height, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \copydoc ReactCollisionBody::AddSphereCollider
			*/
		virtual UniquePtr<Collider> AddSphereCollider(float radius, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \copydoc ReactCollisionBody::GetCollider
			*/
		virtual UniquePtr<Collider> GetCollider() override;
			/**
			 * \copydoc ReactCollisionBody::RemoveCollider
			*/
		virtual void RemoveCollider() override;

			/**
			 * \copydoc ReactCollisionBody::GetCollider
			 */
		virtual void GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation) override;

	private:
		UniquePtr<ReactCollisionBody> m_body;       ///< The ReactCollisionBody associated with the rigid body.
		RigidBody::Type m_type;                     ///< The type of the rigid body.

		// physical properties
		float m_mass{ 0.0f };                       ///< The mass of the rigid body in kilograms.
		float m_linearDamping{ 0.0f };              ///< The linear damping factor of the rigid body, between 0 and 1 incl.
		bool m_hasGravity{ false };                 ///< Specifies if the rigid body has gravity.

		// accelerations
		Math::vec3 m_linearAcceleration{ 0.0f };    ///< The linear acceleration of the body in m/s^2.
		Math::vec3 m_angularAcceleration{ 0.0f };   ///< The angular acceleration of the body in radians/s^2.

		// velocities
		Math::vec3 m_linearVelocity{ 0.0f };        ///< The velocity of the rigid body world space m/s.
		Math::vec3 m_angularVelocity{ 0.0f };       ///< The angular velocity of the rigid body in radians/s.
	};
}
