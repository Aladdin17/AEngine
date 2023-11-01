/**
 * @file
 * @author Christien Alden (34119981)
 * @brief Scene and system implementation
**/
#include "Scene.h"
#include "AEngine/Core/Identifier.h"
#include "AEngine/Core/Logger.h"
#include "AEngine/Core/PerspectiveCamera.h"
#include "AEngine/Messaging/MessageService.h"
#include "AEngine/Physics/PlayerController.h"
#include "AEngine/Skybox/Skybox.h"
#include "AEngine/Water/Water.h"
#include "AEngine/Core/Application.h"
#include "AEngine/Core/Window.h"
#include "AEngine/Render/RenderCommand.h"
#include "AEngine/Render/RenderPipeline.h"
#include "AEngine/Render/UIRenderCommand.h"
#include "Components.h"
#include "Entity.h"
#include "SceneSerialiser.h"
#include <cassert>
#include <fstream>

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
	Scene::Scene(const std::string& ident)
		: m_ident(ident), m_updateStep{ 1.0f / 60.0f }
	{
		UIRenderCommand::Init();
	}

	Scene::~Scene()
	{
		// clear the registry
		PurgeEntitiesStagedForRemoval();
		m_Registry.clear();

		// clear the physics world
		m_physicsWorld.reset();
	}

	void Scene::Init()
	{
		m_physicsWorld = PhysicsAPI::Instance().CreateWorld({ m_updateStep });
	}

	const std::string & Scene::GetIdent() const
	{
		return m_ident;
	}


//--------------------------------------------------------------------------------
// Entity Management
//--------------------------------------------------------------------------------
	Entity Scene::CreateEntity(const std::string& name)
	{
		// add a transform component and tag component to the entity
		Entity entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		TagComponent* tag = entity.AddComponent<TagComponent>();

		if (name.empty())
		{
			tag->tag = "Entity";
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

	void Scene::RemoveEntity(const std::string &tag)
	{
		Entity entt = GetEntity(tag);
		if (entt.IsValid())
		{
			entt.Destroy();
		}
	}

	void Scene::RemoveEntity(Uint16 ident)
	{
		Entity entt = GetEntity(ident);
		if (entt.IsValid())
		{
			entt.Destroy();
		}
	}

	void Scene::GetEntityIds(std::vector<Uint16> &entityids)
	{
		auto entityView = m_Registry.view<TagComponent>();
		for(auto [entity, tc] : entityView.each())
		{
			entityids.push_back(tc.ident);
		}
	}

	void Scene::PurgeEntitiesStagedForRemoval()
	{
		auto it = m_entitiesStagedForRemoval.begin();
		while (it != m_entitiesStagedForRemoval.end())
		{
			// ensure that the entity is still valid, if not, don't need to destroy it
			if (m_Registry.valid(*it))
			{
				m_Registry.destroy(*it);
			}
			// remove the entity from the list of entities staged for removal
			it = m_entitiesStagedForRemoval.erase(it);
		}
	}


//--------------------------------------------------------------------------------
// Events
//--------------------------------------------------------------------------------
	void Scene::OnUpdate(TimeStep dt, bool step)
	{
		TimeStep adjustedDt = 0.0f;
		if (step)
		{
			adjustedDt = m_updateStep;
		}
		else if (m_state == State::Simulate)
		{
			adjustedDt = dt * m_timeScale;
		}

		// update simulation
		MessageService::DispatchMessages();

		ScriptOnUpdate(adjustedDt);
		ScriptOnFixedUpdate(adjustedDt);
		PhysicsOnUpdate(adjustedDt);
		ScriptOnLateUpdate(adjustedDt);

		// purge entities that have been marked for deletion
		PurgeEntitiesStagedForRemoval();

		// render simulation
		PerspectiveCamera* activeCam = m_activeCamera;
		if (s_useDebugCamera)
		{
			s_debugCamera.OnUpdate(dt);
			activeCam = &s_debugCamera;
		}

		// Quick fix for no active camera
		if (activeCam == nullptr)
		{
			auto physicsView = m_Registry.view<CameraComponent>();
			for (auto [entity, cc] : physicsView.each())
			{
				if (cc.defaultCamera)
					activeCam = &cc.camera;
			}
		}

		CameraOnUpdate();

		RenderPipeline::Instance().ClearBuffers();
		RenderPipeline::Instance().BindGeometryPass();
		RenderOpaqueOnUpdate(activeCam);
		AnimateOnUpdate(activeCam, adjustedDt);
		RenderPipeline::Instance().Unbind();
		RenderPipeline::Instance().BindForwardPass();
		RenderPipeline::Instance().LightingPass();
		SkyboxOnUpdate(activeCam);
		RenderTransparentOnUpdate(activeCam);
		//RenderWorldSpaceUI(activeCam);

		if(m_additionalRendering)
		{
			RenderDebugGrid(activeCam);
		}

		if (m_physicsWorld->IsRenderingEnabled())
		{
			m_physicsWorld->Render(activeCam->GetProjectionViewMatrix());
		}

		RenderCommand::EnableDepthTest(false);
		RenderScreenSpaceUI(activeCam);
		RenderCommand::EnableDepthTest(true);

		RenderPipeline::Instance().Unbind();
		RenderPipeline::Instance().TestRender();
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
	void Scene::SetState(State state)
	{
		m_state = state;
	}

	Scene::State Scene::GetState() const
	{
		return m_state;
	}

	void Scene::SetTimeScale(float scale)
	{
		if (scale < 0.0f)
		{
			AE_LOG_ERROR("Scene::SetTimeScale: Time scale must be greater than or equal to zero");
			return;
		}

		m_timeScale = scale;
	}

	float Scene::GetTimeScale() const
	{
		return m_timeScale;
	}

	void Scene::AdvanceOneSimulationStep()
	{
		OnUpdate(m_updateStep, true);
	}

	void Scene::SetRefreshRate(int hertz)
	{
		if (hertz <= 0)
		{
			AE_LOG_ERROR("Scene::SetRefreshRate: Refresh rate must be greater than zero");
			return;
		}

		m_refreshRate = hertz;
		m_updateStep = 1.0f / hertz;
		PhysicsWorld::Props &props = m_physicsWorld->GetProps();
		props.updateStep = m_updateStep;
	}

	int Scene::GetRefreshRate() const
	{
		return m_refreshRate;
	}

	PhysicsWorld* Scene::GetPhysicsWorld() const
	{
		return m_physicsWorld.get();
	}


//--------------------------------------------------------------------------------
// Active Camera Management
//--------------------------------------------------------------------------------
	void Scene::SetActiveCamera(PerspectiveCamera* camera)
	{
		m_activeCamera = camera;
	}

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


//--------------------------------------------------------------------------------
// Runtime Methods
//--------------------------------------------------------------------------------
	void Scene::PhysicsOnUpdate(TimeStep dt)
	{
		// if not in edit mode, update physics
		if (m_state != State::Edit)
		{
			// update physics world
			m_physicsWorld->OnUpdate(dt);

			// get transforms for physics handles
			auto physicsView = m_Registry.view<RigidBodyComponent, TransformComponent>();
			for (auto [entity, rb, tc] : physicsView.each())
			{
				if (rb.ptr)
				{
					if (rb.ptr.get())
					{
						rb.ptr->GetTransform(tc.translation, tc.orientation);
					}
				}
			}

			// get transforms for player controllers
			auto playerControllerView = m_Registry.view<PlayerControllerComponent, TransformComponent>();
			for (auto [entity, pcc, tc] : playerControllerView.each())
			{
				if (pcc.ptr)
				{
					pcc.ptr->OnUpdate(dt);
					tc.translation = pcc.ptr->GetTransform();
				}
			}

			return;
		}

		// if in edit mode, use the transform component to update the physics bodies
		auto physicsView = m_Registry.view<CollisionBodyComponent, TransformComponent>();
		for (auto [entity, cb, tc] : physicsView.each())
		{
			if (cb.ptr)
			{
				cb.ptr->SetTransform(tc.translation, tc.orientation);
			}
		}

		auto rigidBodyView = m_Registry.view<RigidBodyComponent, TransformComponent>();
		for (auto [entity, rb, tc] : rigidBodyView.each())
		{
			if (rb.ptr)
			{
				rb.ptr->SetTransform(tc.translation, tc.orientation);
			}
		}

		auto playerControllerView = m_Registry.view<PlayerControllerComponent, TransformComponent>();
		for (auto [entity, pc, tc] : playerControllerView.each())
		{
			if (pc.ptr)
			{
				pc.ptr->SetTransform(tc.translation, tc.orientation);
			}
		}

		// refresh physics world debug rendering
		m_physicsWorld->ForceRenderingRefresh();
	}

	void Scene::CameraOnUpdate()
	{
		auto cameraView = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto [entity, cameraComp, transformComp] : cameraView.each())
		{
			cameraComp.camera.SetViewMatrix(Math::inverse(transformComp.ToMat4()));
		}
	}

	void Scene::ScriptOnUpdate(TimeStep dt)
	{
		auto scriptView = m_Registry.view<ScriptableComponent>();
		for (auto [entity, script] : scriptView.each())
		{
			script.script->OnUpdate(dt);
		}
	}

	void Scene::ScriptOnFixedUpdate(TimeStep dt)
	{
		static TimeStep accumulator{ 0.0f };
		accumulator += dt;

		// update scripts if enough time has passed
		auto scriptView = m_Registry.view<ScriptableComponent>();
		while (accumulator >= m_updateStep)
		{
			accumulator -= m_updateStep;
			for (auto [entity, script] : scriptView.each())
			{
				script.script->OnFixedUpdate(m_updateStep);
			}
		}
	}

	void Scene::ScriptOnLateUpdate(TimeStep dt)
	{
		auto scriptView = m_Registry.view<ScriptableComponent>();
		for (auto [entity, script] : scriptView.each())
		{
			script.script->OnLateUpdate(dt);
		}
	}

	void Scene::RenderOpaqueOnUpdate(const PerspectiveCamera* activeCam)
	{
		if (activeCam == nullptr)
		{
			return;
		}

		auto renderView = m_Registry.view<RenderableComponent, TransformComponent>();
		for (auto [entity, renderComp, transformComp] : renderView.each())
		{
			// ensure that all needed fields are valid
			if (renderComp.active && renderComp.model && renderComp.shader)
			{
				renderComp.model->RenderOpaque(
					transformComp.ToMat4(), *renderComp.shader, activeCam->GetProjectionViewMatrix()
				);
			}
		}
	}

	void Scene::RenderTransparentOnUpdate(const PerspectiveCamera* activeCam)
	{
		if (activeCam == nullptr)
		{
			return;
		}

		auto renderView = m_Registry.view<RenderableComponent, TransformComponent>();
		for (auto [entity, renderComp, transformComp] : renderView.each())
		{
			if (renderComp.active)
			{
				renderComp.model->RenderTransparent(
					transformComp.ToMat4(), *RenderPipeline::Instance().GetTransparentShader(), activeCam->GetProjectionViewMatrix()
				);
			}
		}
	}

	void Scene::AnimateOnUpdate(const PerspectiveCamera* activeCam, const TimeStep dt)
	{
		if (activeCam == nullptr)
		{
			return;
		}

		auto renderView = m_Registry.view<SkinnedRenderableComponent, TransformComponent>();
		for (auto [entity, renderComp, transformComp] : renderView.each())
		{
			if (renderComp.active)
			{
				renderComp.model->Render(
					transformComp.ToMat4(),
					*renderComp.shader,
					activeCam->GetProjectionViewMatrix(),
					renderComp.animator,
					dt
				);
			}
		}
	}

	void Scene::SkyboxOnUpdate(const PerspectiveCamera* camera)
	{
		if (camera == nullptr)
		{
			return;
		}

		auto skyboxView = m_Registry.view<SkyboxComponent>();
		for (auto [entity, skyboxComp] : skyboxView.each())
		{
			if (skyboxComp.active)
			{
				skyboxComp.skybox->Render(*(skyboxComp.shader), camera->GetProjectionMatrix(), camera->GetViewMatrix());
			}
		}
	}

	void Scene::RenderDebugGrid(const PerspectiveCamera* camera)
	{
		auto panelView = m_Registry.view<NavigationGridComponent>();
		for (auto [entity, navGridComp] : panelView.each())
		{
			if (navGridComp.debug)
			{
				navGridComp.grid->DebugRender(camera);
			}
		}
	}

	void Scene::RenderWorldSpaceUI(const PerspectiveCamera* camera)
	{
		if (camera == nullptr)
		{
			return;
		}

		auto panelView = m_Registry.view<RectTransformComponent, CanvasRendererComponent, PanelComponent>();
		for (auto [entity, rectTransformComp, canvasComp, imgComp] : panelView.each())
		{
			if (canvasComp.active && !canvasComp.screenSpace)
			{
				UIRenderCommand::Render(canvasComp.billboard, false, camera, rectTransformComp.ToMat4(), imgComp.texture, imgComp.color);
			}
		}

		auto textView = m_Registry.view<RectTransformComponent, CanvasRendererComponent, TextComponent>();
		for (auto [entity, rectTransformComp, canvasComp, textComp] : textView.each())
		{
			if (canvasComp.active && !canvasComp.screenSpace)
			{
				textComp.font->Render(canvasComp.billboard, false, camera, textComp.text, rectTransformComp.ToMat4(), textComp.color);
			}
		}
	}

	void Scene::RenderScreenSpaceUI(const PerspectiveCamera* camera)
	{
		if (camera == nullptr)
		{
			return;
		}

		auto panelView = m_Registry.view<RectTransformComponent, CanvasRendererComponent, PanelComponent>();
		for (auto [entity, rectTransformComp, canvasComp, imgComp] : panelView.each())
		{
			if (canvasComp.active && canvasComp.screenSpace)
			{
				UIRenderCommand::Render(canvasComp.billboard, true, camera, rectTransformComp.ToMat4(), imgComp.texture, imgComp.color);
			}
		}

		auto textView = m_Registry.view<RectTransformComponent, CanvasRendererComponent, TextComponent>();
		for (auto [entity, rectTransformComp, canvasComp, textComp] : textView.each())
		{
			if (canvasComp.active && canvasComp.screenSpace)
			{
				textComp.font->Render(canvasComp.billboard, true, camera, textComp.text, rectTransformComp.ToMat4(), textComp.color);
			}
		}
	}
}
