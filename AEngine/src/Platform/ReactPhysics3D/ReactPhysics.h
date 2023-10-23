/**
 * \file   ReactPhysics.h
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#pragma once
#include "AEngine/Core/TimeStep.h"
#include "AEngine/Core/Types.h"
#include "AEngine/Math/Math.h"
#include "AEngine/Physics/CollisionBody.h"
#include "AEngine/Physics/Physics.h"
#include "ReactRenderer.h"
#include <reactphysics3d/reactphysics3d.h>
#include <vector>

	namespace AEngine
	{
		/**
		 * \brief Converts a Math::vec3 to ReactPhysics3D Vector3.
		 *
		 * \param[in] vec The Math::vec3 to convert.
		 * \return The equivalent ReactPhysics3D Vector3.
		 */
	rp3d::Vector3 AEMathToRP3D(const Math::vec3& vec);
		/**
		 * \brief Converts a Math::quat to ReactPhysics3D Quaternion.
		 *
		 * \param[in] quat The Math::quat to convert.
		 * \return The equivalent ReactPhysics3D Quaternion.
		 */
	rp3d::Quaternion AEMathToRP3D(const Math::quat& quat);
		/**
		 * \brief Converts a ReactPhysics3D Vector3 to Math::vec3.
		 *
		 * \param[in] vec The ReactPhysics3D Vector3 to convert.
		 * \return The equivalent Math::vec3.
		 */
	Math::vec3 RP3DToAEMath(const rp3d::Vector3& vec);
		/**
		 * \brief Converts a ReactPhysics3D Quaternion to Math::quat.
		 *
		 * \param[in] quat The ReactPhysics3D Quaternion to convert.
		 * \return The equivalent Math::quat.
		 */
	Math::quat RP3DToAEMath(const rp3d::Quaternion quat);

		/**
		 * \class ReactPhysicsAPI
		 * \brief Represents the ReactPhysics API.
		 *
		 * This class inherits from PhysicsAPI.
		 */
	class ReactPhysicsAPI : public PhysicsAPI
	{
	public:
			/**
			 * \brief Gets the instance of the ReactPhysicsAPI.
			 *
			 * \return The instance of ReactPhysicsAPI.
			 */
		static ReactPhysicsAPI& Instance();
			/**
			 * \brief Creates a new PhysicsWorld.
			 *
			 * \param[in] props The properties for the PhysicsWorld.
			 * \return A pointer to the created PhysicsWorld.
			 */
		virtual UniquePtr<PhysicsWorld> CreateWorld(const PhysicsWorld::Props& props) override;
			/**
			 * \brief Gets the native PhysicsCommon object.
			 *
			 * \return A pointer to the native PhysicsCommon object.
			 */
		rp3d::PhysicsCommon* GetCommon();

	private:
			/**
			 * \brief Constructor for ReactPhysicsAPI.
			 */
		ReactPhysicsAPI();

		rp3d::PhysicsCommon m_common; ///< The native PhysicsCommon object.
	};

		/**
		 * \class ReactPhysicsWorld
		 * \brief Represents a world in ReactPhysics.
		 *
		 * This class inherits from PhysicsWorld.
		 */
	class ReactPhysicsWorld : public PhysicsWorld
	{
	public:
			/**
			 * \brief Constructor for ReactPhysicsWorld.
			 *
			 * \param[in] common The native PhysicsCommon object.
			 */
		ReactPhysicsWorld(rp3d::PhysicsCommon* common);
		virtual ~ReactPhysicsWorld() override;
			/**
			 * \brief Initializes the world with the given settings.
			 *
			 * \param[in] settings The settings for the world.
			 */
		virtual void Init(const Props& settings = Props()) override;
			/**
			 * \brief Sets the update step for the world.
			 * \param[in] step The fixed update step.
			*/
		virtual void SetUpdateStep(TimeStep step) override;
		/**
		 * \brief Called every frame to update the world.
		 *
		 * \param[in] deltaTime The time step for the update.
		 */
		virtual void OnUpdate(TimeStep deltaTime) override;
			/**
			 * \brief Adds a collision body to the world.
			 *
			 * \param[in] position The position of the collision body.
			 * \param[in] orientation The orientation of the collision body.
			 * \return A pointer to the created CollisionBody.
			 */
		virtual SharedPtr<CollisionBody> AddCollisionBody(const Math::vec3& position, const Math::quat& orientation) override;
			/**
			 * \brief Adds a rigid body to the world.
			 *
			 * \param[in] position The position of the rigid body.
			 * \param[in] orientation The orientation of the rigid body.
			 * \return A pointer to the created RigidBody.
			 */
		virtual SharedPtr<RigidBody> AddRigidBody(const Math::vec3& position, const Math::quat& orientation) override;
			/**
			 * \brief Renders the world.
			 *
			 * \param[in] projectionView The projection-view matrix.
			 */
		virtual void Render(const Math::mat4& projectionView) const override;
			/**
			 * \brief Checks if rendering is enabled for the world.
			 *
			 * \return True if rendering is enabled, false otherwise.
			 */
		virtual bool IsRenderingEnabled() const override;
			/**
			 * \brief Sets whether rendering is enabled for the world.
			 *
			 * \param enable True to enable rendering, false to disable.
			 */
		virtual void SetRenderingEnabled(bool enable) override;
			/**
			 * \brief Gets the renderer for the world.
			 *
			 * \return A pointer to the ReactPhysicsRenderer.
			 */
		virtual const ReactPhysicsRenderer* GetRenderer() const override;
			/**
			 * \brief Gets the native PhysicsWorld object.
			 *
			 * \return A pointer to the native PhysicsWorld object.
			 */
		rp3d::PhysicsWorld* GetNative();
			/**
			 * \brief Gets the value of the accumulator.
			 *
			 * \return The value of the accumulator.
			 */
		float GetAccumulatorVal() const { return m_accumulator; }
			/**
			 * \brief Gets the update step value.
			 *
			 * \return The update step value.
			 */
		float GetUpdateStep() const { return m_updateStep; }
		virtual void ForceRenderingRefresh() override;

	private:
		rp3d::PhysicsWorld* m_world;                             ///< The native PhysicsWorld object.
		UniquePtr<ReactPhysicsRenderer> m_renderer;              ///< The ReactPhysicsRenderer.
		TimeStep m_accumulator;                                  ///< The value of the accumulator.
		TimeStep m_updateStep;                                   ///< The update step value.

		std::vector<WeakPtr<CollisionBody>> m_collisionBodies;   ///< The collision bodies in the world
		std::vector<WeakPtr<RigidBody>> m_rigidBodies;           ///< The rigid bodies in the world, used to run the update step.
	};
}
