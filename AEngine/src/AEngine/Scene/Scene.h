#pragma once
#include <stack>
#include <string>
#include <EnTT/entt.hpp>
#include <yaml-cpp/yaml.h>
#include "AEngine/Core/TimeStep.h"
#include "AEngine/Core/Types.h"
#include "AEngine/Physics/Physics.h"
#include "Components.h"
#include "DebugCamera.h"

namespace AEngine
{
	class Entity;

	/**
		 * \class Scene
		 * \brief Scene Class used by the ECS
		 * \author Geoff Candy (34183006)
		 * \author Christien Alden (34119981)
		**/
	class Scene
	{
	public:
//--------------------------------------------------------------------------------
// Initialisation and Management
//--------------------------------------------------------------------------------
			/**
			 * \brief Gets the scene identifier
			 * \return The identifier of the scene
			*/
		const std::string& GetIdent() const;

			/**
			 * \brief Method to create Entities within the Scene
			 * \param[in] name of entity; default is "DefaultEntity"
			 * \return Entity
			**/
		Entity CreateEntity(const std::string& name = std::string());
			/**
			 * \brief Method to retrieve an Entity from the Scene
			 * \param[in] tag of entity
			 * \return Entity
			 * \note Check the Entity::IsValid() method before using the Entity
			*/
		Entity GetEntity(const std::string& tag);
			/**
			 * \brief Method to retrieve an Entity from the Scene
			 * \param[in] ident of entity
			 * \return Entity
			 * \note Check the Entity::IsValid() method before using the Entity
			*/
		Entity GetEntity(Uint16 ident);

		void RemoveEntity(const std::string& tag);
		void RemoveEntity(Uint16 ident);

//--------------------------------------------------------------------------------
// Events
//--------------------------------------------------------------------------------
			/**
			 * \brief Updates the scene during runtime
			 * \details
			 * This should be called each frame just before the window is refreshed.
			**/
		void OnUpdate(TimeStep dt);

			/**
			 * \brief Updates scene cameras to reflect the new aspect ratio
			 * \param[in] width of scene
			 * \param[in] height of scene
			**/
		void OnViewportResize(unsigned int width, unsigned int height);

//--------------------------------------------------------------------------------
// Simulation
//--------------------------------------------------------------------------------
			/**
			 * \brief Resumes scene simulation
			**/
		void Start();

			/**
			 * \brief Pauses scene simulation
			**/
		void Stop();

			/**
			 * \brief Returns running state of simulation
			 * \retval true if simulation is running
			 * \retval false if simulation is **not** running
			**/
		bool IsRunning();

			/**
			 * \brief Sets the active camera for the scene
			 * \param[in] camera to set as active
			**/
		void SetActiveCamera(PerspectiveCamera* camera);

//--------------------------------------------------------------------------------
// Debug Camera
//--------------------------------------------------------------------------------
			/**
			 * \brief Sets whether the scene uses the debug camera or an entity camera
			 * \param[in] value true to use debug cam; false to revert to entity cam
			 * \bug Currently controllable entities are still updated
			**/
		static void UseDebugCamera(bool value);

			/**
			 * \brief Returns status of Scene::SetDebugCamera
			 * \retval true if using debug camera
			 * \retval false if **not** using debug camera
			**/
		static bool UsingDebugCamera();

			/**
			 * \brief Returns the debug camera reference to update properties
			 * \return DebugCamera&
			**/
		static DebugCamera& GetDebugCamera();

//--------------------------------------------------------------------------------
	private:
		friend class Entity;
		friend class SceneManagerImpl;
		friend class SceneSerialiser;

		// core
		std::string m_ident;
		bool m_isRunning;
		TimeStep m_fixedTimeStep;
		PerspectiveCamera* m_activeCamera;
		entt::registry m_Registry;
		PhysicsWorld* m_physicsWorld;

//--------------------------------------------------------------------------------
// Debug Camera
//--------------------------------------------------------------------------------
		static DebugCamera s_debugCamera;
		static bool s_useDebugCamera;

//--------------------------------------------------------------------------------
// Runtime Methods
//--------------------------------------------------------------------------------
			/**
			 * \brief Constructor for Scene
			 * \param[in] ident of scene; default is "Default Scene"
			 * \note There can only be one scene with a given ident
			 * \warning This should not be called directly, use SceneManager::CreateScene() instead
			*/
		Scene(const std::string& ident = "Default Scene");
			/**
			 * \brief Initialises scene
			**/
		void Init();
			/**
			 * \brief Updates the cameras in the scene
			 * \todo Refactor this to only update the active camera
			 * \remark Will have to be integrated in with the OnViewportResize method
			*/
		void CameraOnUpdate();
			/**
			 * \brief Calls modern render system with the given camera
			 * \param[in] camera to render scene from
			**/
		void RenderOnUpdate(const PerspectiveCamera* camera);
			/**
			 * \brief Calls modern terrain system with the given camera
			 * \param[in] camera to render scene from
			*/
		void TerrainOnUpdate(const PerspectiveCamera* camera);
			/**
			 * \brief Updates physics in scene
			 * \param[in] dt timestep
			*/
		void PhysicsOnUpdate(TimeStep dt);
			/**
			 * \brief Updates scripts in scene
			 * \param[in] dt timestep
			*/
		void ScriptOnUpdate(TimeStep dt);
		void ScriptOnFixedUpdate(TimeStep dt);
		void ScriptOnLateUpdate(TimeStep dt);
	};
}
