/**
 * @file
 * @author Christien Alden (34119981)
 * @brief Provides an entry point for entire project
**/
#include <AEngine.h>
#include <imgui.h>
#include <cstdlib>
#include <memory>

class DemoLayer : public AEngine::Layer
{
public:
	DemoLayer(const std::string& ident)
		: AEngine::Layer(ident)
	{
	}

	void OnAttach() override
	{
		// load scenes
		AEngine::Scene *level1 = AEngine::SceneManager::LoadFromFile("assets/scenes/level1.scene");
		if (!level1)
		{
			exit(1);
		}

		// set active scene and debug camera
		AEngine::SceneManager::SetActiveScene("level1");
		AEngine::Scene::UseDebugCamera(true);
		AEngine::DebugCamera& debugCam = AEngine::Scene::GetDebugCamera();
		debugCam.SetFarPlane(10000.0f);
		debugCam.SetMovementSpeed(20.0f);
		debugCam.SetNearPlane(0.1f);
		debugCam.SetFov(45.0f);
		debugCam.SetYaw(-90.0f);

		// set default camera
		AEngine::CameraComponent* camComp = AEngine::SceneManager::GetActiveScene()->GetEntity("Player").GetComponent<AEngine::CameraComponent>();
		AEngine::SceneManager::GetActiveScene()->SetActiveCamera(&camComp->camera);

		// setup physics debug renderer
		const AEngine::PhysicsRenderer* debugRenderer = AEngine::SceneManager::GetActiveScene()->GetPhysicsRenderer();
		debugRenderer->SetRenderItem(AEngine::PhysicsRendererItem::CollisionShape, true);
		debugRenderer->SetRenderItem(AEngine::PhysicsRendererItem::ContactPoint, true);
		debugRenderer->SetRenderShape(AEngine::CollisionRenderShape::Capsule, true);
		debugRenderer->SetRenderShape(AEngine::CollisionRenderShape::Box, true);
	}

	void OnDetach() override
	{
		// do nothing
	}

	void OnUpdate(AEngine::TimeStep ts) override
	{
		if (AEngine::Input::IsKeyPressedNoRepeat(AEKey::F1))
		{
			AEngine::RenderCommand::PolygonMode(AEngine::PolygonFace::FrontAndBack, AEngine::PolygonDraw::Fill);
		}

		if (AEngine::Input::IsKeyPressedNoRepeat(AEKey::F2))
		{
			AEngine::RenderCommand::PolygonMode(AEngine::PolygonFace::FrontAndBack, AEngine::PolygonDraw::Line);
		}

		if (AEngine::Input::IsKeyPressedNoRepeat(AEKey::F3))
		{
			AEngine::RenderCommand::PolygonMode(AEngine::PolygonFace::FrontAndBack, AEngine::PolygonDraw::Point);
		}

		if (AEngine::Input::IsKeyPressedNoRepeat(AEKey::F4))
		{
			if (AEngine::SceneManager::GetActiveScene()->IsPhysicsRenderingEnabled())
			{
				AEngine::SceneManager::GetActiveScene()->SetPhysicsRenderingEnabled(false);
			}
			else
			{
				AEngine::SceneManager::GetActiveScene()->SetPhysicsRenderingEnabled(true);
			}
		}

		if (AEngine::Input::IsKeyPressedNoRepeat(AEKey::F5))
		{
			if (AEngine::SceneManager::GetActiveScene()->UsingDebugCamera())
			{
				AEngine::SceneManager::GetActiveScene()->UseDebugCamera(false);
			}
			else
			{
				AEngine::SceneManager::GetActiveScene()->UseDebugCamera(true);
			}
		}


		if (AEngine::Input::IsKeyPressedNoRepeat(AEKey::P))
		{
			if (AEngine::Application::Instance().GetWindow()->IsShowingCursor())
			{
				AEngine::Application::Instance().GetWindow()->ShowCursor(false);
				AEngine::Application::Instance().EditMode(false);
			}
			else
			{
				AEngine::Application::Instance().GetWindow()->ShowCursor(true);
				AEngine::Application::Instance().EditMode(true);
			}
		}

		AEngine::SceneManager::GetActiveScene()->OnUpdate(ts);
	}
};

class DemoApp : public AEngine::Application
{
public:
	DemoApp(AEngine::Application::Properties props)
		: Application{ props }
	{
		SetLayer(std::make_unique<DemoLayer>("Test Layer"));
		this->GetWindow()->ShowCursor(false);

		// setup render settings
		AEngine::RenderCommand::EnableBlend(true);
		AEngine::RenderCommand::SetBlendFunction(AEngine::BlendFunction::SourceAlpha, AEngine::BlendFunction::OneMinusSourceAlpha);
		AEngine::RenderCommand::EnableFaceCulling(true);
		AEngine::RenderCommand::SetCullFace(AEngine::PolygonFace::Back);
		this->GetWindow()->Maximise();
	}
};

AEngine::Application* AEngine::CreateApplication(AEngine::Application::Properties& props)
{
	props.title = "Scavenger Hunt";
	return new DemoApp(props);
}