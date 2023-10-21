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
			 *
			 * \param[in] world The ReactPhysicsWorld associated with the collision body.
			 * \param[in] position The initial position of the collision body.
			 * \param[in] orientation The initial orientation of the collision body.
			 * \param[in] isRigid Specifies if the collision body is rigid (default: false).
			 */
		ReactCollisionBody(ReactPhysicsWorld* world, const Math::vec3& position, const Math::quat& orientation, bool isRigid = false);

		virtual ~ReactCollisionBody();
			/**
			 * \brief Sets the transform (position and orientation) of the collision body.
			 *
			 * \param[in] position The new position of the collision body.
			 * \param[in] orientation The new orientation of the collision body.
			 */
		virtual void SetTransform(const Math::vec3&, const Math::quat&) override;
			/**
			 * \brief Gets the transform (position and orientation) of the collision body.
			 *
			 * \param[out] position The position of the collision body.
			 * \param[out] orientation The orientation of the collision body.
			 */
		virtual void GetTransform(Math::vec3&, Math::quat&) const override;
			/**
			 * \brief Adds a box collider to the collision body.
			 *
			 * \param[in] size The size of the box collider.
			 * \return A pointer to the created Collider object.
			 */
		virtual UniquePtr<Collider> AddBoxCollider(const Math::vec3& size, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
		virtual UniquePtr<Collider> AddCapsuleCollider(float radius, float height, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
		virtual UniquePtr<Collider> AddSphereCollider(float radius, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \brief Gets the interpolated transform (position and orientation) of the collision body.
			 *
			 * \param[out] position The interpolated position of the collision body.
			 * \param[out] orientation The interpolated orientation of the collision body.
			 */
		virtual void GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation) override;
		virtual UniquePtr<Collider> GetCollider() override;
		virtual void RemoveCollider() override;
			/**
			 * \brief Returns the native collision body object.
			 *
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
			 *
			 * \param world The ReactPhysicsWorld associated with the rigid body.
			 * \param position The initial position of the rigid body.
			 * \param orientation The initial orientation of the rigid body.
			 */
		ReactRigidBody(ReactPhysicsWorld* world, const Math::vec3& position, const Math::quat& orientation);
			/**
			 * \brief Default destructor.
			 */
		virtual ~ReactRigidBody() = default;
			/**
			 * \brief Sets whether the rigid body has gravity or not.
			 *
			 * \param hasGravity Specifies if the rigid body has gravity.
			 */
		virtual void SetHasGravity(bool hasGravity) override;
			/**
			 * \brief Returns whether the rigid body has gravity or not.
			 *
			 * \return True if the rigid body has gravity, false otherwise.
			 */
		virtual bool GetHasGravity() const override;
			/**
			 * \brief Sets the velocity of the rigid body.
			 *
			 * \param velocity The new velocity of the rigid body.
			 */
		virtual void SetVelocity(const Math::vec3& velocity) override;
			/**
			 * \brief Returns the velocity of the rigid body.
			 *
			 * \return The velocity of the rigid body.
			 */
		virtual const Math::vec3 GetVelocity() const override;
			/**
			 * \brief Sets the drag factor of the rigid body.
			 *
			 * \param factor The drag factor of the rigid body.
			 */
		virtual void SetDrag(float factor) override;
			/**
			 * \brief Sets the mass of the rigid body.
			 *
			 * \param massKg The mass of the rigid body in kilograms.
			 */
		virtual void SetMass(float massKg) override;
			/**
			 * \brief Returns the mass of the rigid body.
			 *
			 * \return The mass of the rigid body in kilograms.
			 */
		virtual float GetMass() const override;

		//--------------------------------------------------------------------------------
		// From CollisionBody
		//--------------------------------------------------------------------------------
			/**
			 * \brief Sets the transform (position and orientation) of the rigid body.
			 *
			 * \param[in] position The new position of the rigid body.
			 * \param[in] orientation The new orientation of the rigid body.
			 */
		virtual void SetTransform(const Math::vec3& position, const Math::quat& orientation) override;
			/**
			 * \brief Gets the transform (position and orientation) of the rigid body.
			 *
			 * \param[out] position The position of the rigid body.
			 * \param[out] orientation The orientation of the rigid body.
			 */
		virtual void GetTransform(Math::vec3& position, Math::quat& orientation) const override;
			/**
			 * \brief Adds a box collider to the rigid body.
			 *
			 * \param[in] size The size of the box collider.
			 * \return A pointer to the created Collider object.
			 */
		virtual UniquePtr<Collider> AddBoxCollider(const Math::vec3& size, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
		virtual UniquePtr<Collider> AddCapsuleCollider(float radius, float height, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
		virtual UniquePtr<Collider> AddSphereCollider(float radius, const Math::vec3& offset = Math::vec3{0.0f}, const Math::quat& orientation = Math::quat{Math::vec3{ 0.0f, 0.0f, 0.0f }}) override;
			/**
			 * \brief Gets the interpolated transform (position and orientation) of the rigid body.
			 *
			 * \param[out] position The interpolated position of the rigid body.
			 * \param[out] orientation The interpolated orientation of the rigid body.
			 */
		virtual void GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation) override;
			/**
			 * \brief Sets the type of the rigid body.
			 *
			 * \param[in] type The type of the rigid body.
			 */
		virtual void SetType(Type type) override;
		virtual Type GetType() const override;
		virtual UniquePtr<Collider> GetCollider() override;
		virtual void RemoveCollider() override;
			/**
			 * \brief Returns the native rigid body object.
			 *
			 * \return A pointer to the native ReactPhysics3D RigidBody object.
			 */
		rp3d::RigidBody* GetNative() const;

	private:
		UniquePtr<ReactCollisionBody> m_body; ///< The ReactCollisionBody associated with the rigid body.
	};
}
