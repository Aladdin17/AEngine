/**
 * \file   Physics.h
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#pragma once
#include "AEngine/Core/TimeStep.h"
#include "CollisionBody.h"
#include "AEngine/Math/Math.h"
#include "Renderer.h"

namespace AEngine
{
		/**
		 * \class PhysicsWorld
		 * \brief Represents a physics world.
		 * \author Lane O'Rafferty (33534304)
		*/
	class PhysicsWorld
	{
	public:
			/**
			 * \struct Props
			 * \brief Contains properties for configuring the physics world.
			 */
		struct Props
		{
			TimeStep updateStep{ 1.0f / 60.0f }; ///< The time step for physics simulation updates.
		};

		virtual ~PhysicsWorld() = default;
			/**
			 * \brief Initializes the physics world with the specified settings.
			 * \param[in] settings The settings to initialize the physics world with.
			 */
		virtual void Init(const Props& settings) = 0;
			/**
			 * \brief Updates the physics world.
			 * \param[in] deltaTime The time step for the update.
			 */
		virtual void OnUpdate(TimeStep deltaTime) = 0;
			/**
			 * \brief Adds a collision body to the physics world.
			 * \param[in] position The position of the collision body.
			 * \param[in] orientation The orientation of the collision body.
			 * \return A pointer to the created CollisionBody object.
			 */
		virtual CollisionBody* AddCollisionBody(const Math::vec3& position, const Math::quat& orientation) = 0;
			/**
			 * \brief Adds a rigid body to the physics world.
			 * \param[in] position The position of the rigid body.
			 * \param[in] orientation The orientation of the rigid body.
			 * \return A pointer to the created RigidBody object.
			 */
		virtual RigidBody* AddRigidBody(const Math::vec3& position, const Math::quat& orientation) = 0;

		// rendering
		virtual void Render(const Math::mat4& projectionView) const = 0;
		virtual bool IsRenderingEnabled() const = 0;
		virtual void SetRenderingEnabled(bool enable) = 0;
		virtual const PhysicsRenderer* GetRenderer() const = 0;
	};

		/**
		 * \class PhysicsAPI
		 * \brief Represents a physics API.
		 */
	class PhysicsAPI
	{
	public:
			/**
			 * \brief Gets the instance of the PhysicsAPI.
			 * \return The instance of the PhysicsAPI.
			 */
		static PhysicsAPI& Instance();
			/**
			 * \brief Creates a new physics world with the specified properties.
			 * \param[in] props The properties for configuring the physics world (optional).
			 * \return A pointer to the created PhysicsWorld object.
			 */
		virtual PhysicsWorld* CreateWorld(const PhysicsWorld::Props& props = PhysicsWorld::Props()) = 0;
			/**
			 * \brief Destroys a physics world.
			 * \param[out] world The PhysicsWorld object to destroy.
			 */
		virtual void DestroyWorld(PhysicsWorld* world) = 0;

	protected:
		PhysicsAPI() = default;
	};
}
