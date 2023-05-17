/**
 * @file
 * @author Christien Alden (34119981)
 * @brief Scene and system implementation
**/
#include <cassert>
#include <fstream>
#include "AEngine/Core/Identifier.h"
#include "AEngine/Core/Logger.h"
#include "AEngine/Core/PerspectiveCamera.h"
#include "AEngine/Render/Renderer.h"
#include "Components.h"
#include "Entity.h"
#include "Scene.h"
#include "SceneSerialiser.h"

namespace AEngine
{
//--------------------------------------------------------------------------------
// Static Initialisation
//--------------------------------------------------------------------------------
	DebugCamera Scene::s_debugCamera = DebugCamera();
	bool Scene::s_useDebugCamera = false;

//--------------------------------------------------------------------------------
// Initialisation and Management
//--------------------------------------------------------------------------------
	const std::string & Scene::GetIdent() const
	{
		return m_ident;
	}

	Scene::Scene(const std::string& ident)
		: m_ident(ident)
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity(m_Registry.create(), this);
		TagComponent* tag = entity.AddComponent<TagComponent>();
		if (name.empty())
		{
			tag->tag = "DefaultEntity";
		}
		else
		{
			tag->tag = name;
		}

		tag->ident = Identifier::Generate();
		return entity;
	}

	Entity Scene::GetEntity(const std::string& tag)
	{
		auto entityView = m_Registry.view<TagComponent>();
		for (auto [entity, TagComponent] : entityView.each())
		{
			if (TagComponent.tag == tag)
			{
				return Entity(entity, this);
			}
		}

		return Entity(entt::null, this);
	}

	Entity Scene::GetEntity(Uint16 ident)
	{
		auto entityView = m_Registry.view<TagComponent>();
		for (auto [entity, TagComponent] : entityView.each())
		{
			if (TagComponent.ident == ident)
			{
				return Entity(entity, this);
			}
		}

		return Entity(entt::null, this);
	}

//--------------------------------------------------------------------------------
// Events
//--------------------------------------------------------------------------------
	void Scene::Init(unsigned int updatesPerSecond)
	{
		if (updatesPerSecond == 0)
		{
			AE_LOG_FATAL("Scene::Init::Failed -> updatesPerSecond must not be zero");
		}

		m_isRunning = false;
		m_physicsWorld = PhysicsAPI::Instance().CreateWorld({ 1.0f / updatesPerSecond });

		/// \todo Move this to a better place!
		auto rigidBodyView = m_Registry.view<RigidBodyComponent, TransformComponent>();
		for (auto [entity, rbc, tc] : rigidBodyView.each())
		{
			rbc.ptr = m_physicsWorld->AddRigidBody(tc.translation, tc.orientation);
			rbc.ptr->SetHasGravity(rbc.hasGravity);
			rbc.ptr->SetMass(rbc.massKg);

			PhysicsHandle& handle = m_Registry.emplace<PhysicsHandle>(entity);
			handle.ptr = dynamic_cast<CollisionBody*>(rbc.ptr);
		}

		auto boxColliderView = m_Registry.view<BoxColliderComponent, TransformComponent>();
		for (auto [entity, bcc, tc] : boxColliderView.each())
		{
			if (m_Registry.all_of<PhysicsHandle>(entity))
			{
				PhysicsHandle& handle = m_Registry.get<PhysicsHandle>(entity);
				bcc.ptr = handle.ptr->AddBoxCollider(bcc.size);
				bcc.ptr->SetIsTrigger(bcc.isTrigger);
			}
			else
			{
				PhysicsHandle& handle = m_Registry.emplace<PhysicsHandle>(entity);
				handle.ptr = m_physicsWorld->AddCollisionBody(tc.translation, tc.orientation);
				bcc.ptr = handle.ptr->AddBoxCollider(bcc.size);
				bcc.ptr->SetIsTrigger(bcc.isTrigger);
			}
		}
	}

	void Scene::OnUpdate(TimeStep dt)
	{
		if (IsRunning())
		{
			m_physicsWorld->OnUpdate(dt);
			PhysicsOnUpdate();
			ScriptableOnUpdate(dt);
		}

		PerspectiveCamera* activeCam = nullptr;
		if (s_useDebugCamera)
		{
			s_debugCamera.OnUpdate(dt);
			activeCam = &s_debugCamera;
		}
		else
		{
			activeCam = CamerasOnUpdate();
		}

		if (activeCam != nullptr)
		{
			RenderOnUpdate(*activeCam);
			TerrainOnUpdate(*activeCam);
		}
	}

	void Scene::OnViewportResize(unsigned int width, unsigned int height)
	{
		// avoid divide by zero error
		if (height == 0) { height = 1; }

		// update each camera's aspect ratio
		auto cameraView = m_Registry.view<CameraComponent>();
		for (auto [entity, cameraComp] : cameraView.each())
		{
			cameraComp.camera.SetAspect((float) width / height);
		}
	}

//--------------------------------------------------------------------------------
// Simulation
//--------------------------------------------------------------------------------
	void Scene::Start()
	{
		AE_LOG_DEBUG("Scene::Start");
		m_isRunning = true;
	}

	void Scene::Stop()
	{
		AE_LOG_DEBUG("Scene::Stop");
		m_isRunning = false;
	}

	bool Scene::IsRunning()
	{
		return m_isRunning;
	}

	bool Scene::SetActiveCamera(const std::string& entityTag)
	{
		// only set one camera as active camera
		CameraComponent* lastActive{ nullptr };
		bool found{ false };

		auto cameraEntityView = m_Registry.view<CameraComponent, TagComponent>();
		for (auto [entity, cameraComp, tagComp] : cameraEntityView.each())
		{
			// save old active camera
			if (cameraComp.active)
			{
				lastActive = &cameraComp;
			}

			// update if found
			if (entityTag == tagComp.tag)
			{
				cameraComp.active = true;
				found = true;
			}
			else // set all others to false
			{
				cameraComp.active = false;
			}
		}

		// reset last active if new active not found
		if (!found && lastActive)
		{
			lastActive->active = true;
		}

		return found;
	}

//--------------------------------------------------------------------------------
// Runtime Methods
//--------------------------------------------------------------------------------
	PerspectiveCamera* Scene::CamerasOnUpdate()
	{
		PerspectiveCamera* activeCam{ nullptr };
		auto cameraView = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto [entity, cameraComp, transformComp] : cameraView.each())
		{
			if (cameraComp.active)
			{
				cameraComp.camera.SetViewMatrix(Math::inverse(transformComp.ToMat4()));
				activeCam = &cameraComp.camera;
				break;
			}
		}

		return activeCam;
	}

	void Scene::PhysicsOnUpdate()
	{
		auto physicsView = m_Registry.view<PhysicsHandle, TransformComponent>();
		for (auto [entity, ph, tc] : physicsView.each())
		{
			if (ph.ptr)
			{
				ph.ptr->GetTransform(tc.translation, tc.orientation);
			}
		}
	}

	void Scene::RenderOnUpdate(const PerspectiveCamera& activeCam)
	{
		Renderer* renderer = Renderer::Instance();

		// set the new projection view matrix
		renderer->SetProjection(activeCam.GetProjectionViewMatrix());

		auto renderView = m_Registry.view<RenderableComponent, TransformComponent>();
		for (auto [entity, renderComp, transformComp] : renderView.each())
		{
			if (renderComp.active)
			{
				renderer->Submit(
					*renderComp.model,*renderComp.shader, transformComp.ToMat4()
				);
			}
		}
	}

	void Scene::ScriptableOnUpdate(TimeStep dt)
	{
		auto scriptView = m_Registry.view<ScriptableComponent>();
		for (auto [entity, script] : scriptView.each())
		{
			script.script->OnUpdate(dt, Entity(entity,this));
		}
	}

	void Scene::TerrainOnUpdate(const PerspectiveCamera& activeCam)
	{
		Renderer* renderer = Renderer::Instance();

		// set the new projection view matrix
		renderer->SetProjection(activeCam.GetProjectionViewMatrix());

		auto renderView = m_Registry.view<TerrainComponent, TransformComponent>();
		for (auto [entity, terrainComp, transformComp] : renderView.each())
		{
			if (terrainComp.active)
			{
				renderer->SubmitTerrain(
					terrainComp.textures, terrainComp.yRange, *terrainComp.terrain, *terrainComp.shader, transformComp.ToMat4()
				);
			}
		}
	}

//--------------------------------------------------------------------------------
// Debug Camera
//--------------------------------------------------------------------------------
	void Scene::UseDebugCamera(bool value)
	{
		s_useDebugCamera = value;
	}

	bool Scene::UsingDebugCamera()
	{
		return s_useDebugCamera;
	}

	DebugCamera& Scene::GetDebugCamera()
	{
		return s_debugCamera;
	}
}
