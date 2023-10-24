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
		using namespace AEngine;

		// load scenes
		Scene *physicsScene = SceneManager::LoadFromFile("assets/scenes/physics.scene");
		if (!physicsScene)
		{
			exit(1);
		}

		// set active scene and debug camera
		SceneManager::SetActiveScene("physics");
		Scene::UseDebugCamera(true);
		DebugCamera& debugCam = Scene::GetDebugCamera();
		debugCam.SetFarPlane(10000.0f);
		debugCam.SetMovementSpeed(20.0f);
		debugCam.SetNearPlane(0.1f);
		debugCam.SetFov(45.0f);
		debugCam.SetYaw(-90.0f);

		// setup physics debug renderer
		const PhysicsRenderer* debugRenderer = physicsScene->GetPhysicsRenderer();
		debugRenderer->SetRenderItem(PhysicsRendererItem::CollisionShape, true);
		debugRenderer->SetRenderItem(PhysicsRendererItem::ContactPoint, true);
		debugRenderer->SetRenderShape(CollisionRenderShape::Capsule, true);
		debugRenderer->SetRenderShape(CollisionRenderShape::Box, true);

		// set scene to simulation mode and turn on physics rendering
		physicsScene->SetState(Scene::State::Simulate);
		physicsScene->SetPhysicsRenderingEnabled(true);
	}

	void OnDetach() override
	{
		// do nothing
	}

	void OnUpdate(AEngine::TimeStep ts) override
	{
		if(AEngine::Input::IsMouseButtonPressedNoRepeat(AEMouse::BUTTON_LEFT))
		{
			using namespace AEngine;
			DebugCamera& cam = Scene::GetDebugCamera();
			Math::vec3 pos = cam.GetPosition();
			Math::vec3 front = cam.GetFront();
			const float projectileSpeed = 20.0f;
			Math::vec3 projectileVelocity = front * projectileSpeed;

			Entity projectile = SceneManager::GetActiveScene()->CreateEntity("projectile");

			TransformComponent* transform = projectile.AddComponent<TransformComponent>();
			transform->translation = pos;
			transform->orientation = Math::quat{ Math::vec3{0.0f, 0.0f, 0.0f} };
			transform->scale = Math::vec3{ 1.0f };

			RigidBodyComponent* rigidBody = projectile.AddComponent<RigidBodyComponent>();
			rigidBody->ptr = SceneManager::GetActiveScene()->GetPhysicsWorld()->AddRigidBody(pos, Math::quat{ Math::vec3{0.0f, 0.0f, 0.0f} });
			rigidBody->ptr->SetMass(0.5f);
			rigidBody->ptr->SetLinearVelocity(projectileVelocity);
			rigidBody->ptr->AddSphereCollider(0.5f);

			RenderableComponent* render = projectile.AddComponent<RenderableComponent>();
			render->active = true;
			render->model = AssetManager<Model>::Instance().Get("sphere.gltf");
			render->shader = AssetManager<Shader>::Instance().Get("simple.shader");

			ScriptableComponent* scriptComponent = projectile.AddComponent<ScriptableComponent>();
			Script* script = AssetManager<Script>::Instance().Get("projectile.lua").get();
			scriptComponent->script = MakeUnique<EntityScript>(projectile, ScriptEngine::GetState(), script);
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

		if (AEngine::Input::IsKeyPressedNoRepeat(AEKey::ESCAPE))
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
		SetLayer(std::make_unique<DemoLayer>("Physics Demo"));
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
	props.title = "Geoff's Physics Demo";
	return new DemoApp(props);
}
