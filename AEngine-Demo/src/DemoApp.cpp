/**
 * @file
 * @author Christien Alden (34119981)
 * @brief Provides an entry point for entire project
**/
#include <AEngine.h>
#include <imgui.h>
#include <cstdlib>
#include <memory>

namespace {
	void FireProjectile(
		AEngine::Math::vec3 startingPosition,
		AEngine::Math::vec3 normalizedDirection)
	{
		using namespace AEngine;
		static int projectileCount = 0;
		constexpr float radius = 0.5f;        // 0.5m
		constexpr float speed = 100.0f;       // 100m/s
		constexpr float mass = 1.0f;          // 1kg
		constexpr float restitution = 0.6f;

		// create the projectile
		Scene* activeScene = SceneManager::GetActiveScene();
		Entity projectile = activeScene->CreateEntity("projectile" + std::to_string(projectileCount));

		// add the transform
		projectile.AddComponent<TransformComponent>(
			startingPosition,
			Math::quat{ Math::vec3{0.0f, 0.0f, 0.0f} },
			Math::vec3{ radius * 2.0f }
		);

		// add the renderable component
		projectile.AddComponent<RenderableComponent>(
			true,
			AssetManager<Model>::Instance().Get("sphere.gltf"),
			AssetManager<Shader>::Instance().Get("simple.shader")
		);

		// add the rigid body component
		RigidBodyComponent *rigidBodyComp = projectile.AddComponent<RigidBodyComponent>();
		rigidBodyComp->ptr = SceneManager::GetActiveScene()->GetPhysicsWorld()->AddRigidBody(
			startingPosition,
			Math::quat{ Math::vec3{0.0f, 0.0f, 0.0f} }
		);

		// set the rigidbody properties
		rigidBodyComp->ptr->SetMass(mass);
		rigidBodyComp->ptr->SetCentreOfMass(Math::vec3{ 0.0f, 0.0f, 0.0f });
		rigidBodyComp->ptr->SetLinearVelocity(normalizedDirection * speed);
		rigidBodyComp->ptr->SetType(RigidBody::Type::Dynamic);
		rigidBodyComp->ptr->SetHasGravity(true);
		rigidBodyComp->ptr->SetRestitution(restitution);

		// add a collider to the rigid body
		rigidBodyComp->ptr->AddSphereCollider(radius);

		// attach script to destroy the projectile
		ScriptableComponent* scriptComp = projectile.AddComponent<ScriptableComponent>();
		Script* script = AssetManager<Script>::Instance().Get("projectile.lua").get();
		scriptComp->script = MakeUnique<EntityScript>(projectile, ScriptEngine::GetState(), script);

		++projectileCount;
	}
}

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
		Scene *physicsScene = SceneManager::LoadFromFile("assets/scenes/serialized.scene");
		if (!physicsScene)
		{
			exit(1);
		}

		// set active scene and debug camera
		SceneManager::SetActiveScene("serialized");
		Scene::UseDebugCamera(true);
		DebugCamera& debugCam = Scene::GetDebugCamera();
		debugCam.SetFarPlane(10000.0f);
		debugCam.SetMovementSpeed(20.0f);
		debugCam.SetNearPlane(0.1f);
		debugCam.SetFov(45.0f);
		debugCam.SetYaw(-90.0f);

		// setup physics debug renderer
		PhysicsWorld* physicsWorld = physicsScene->GetPhysicsWorld();
		physicsWorld->SetRenderingEnabled(true);
		const PhysicsRenderer* debugRenderer = physicsWorld->GetRenderer();
		debugRenderer->SetRenderItem(PhysicsRendererItem::CollisionShape, true);
		debugRenderer->SetRenderItem(PhysicsRendererItem::ColliderAABB, true);
		debugRenderer->SetRenderItem(PhysicsRendererItem::ContactPoint, true);
		debugRenderer->SetRenderItem(PhysicsRendererItem::ContactNormal, true);
		debugRenderer->SetRenderShape(CollisionRenderShape::Box, true);
		debugRenderer->SetRenderShape(CollisionRenderShape::Sphere, true);

		// set scene to simulation mode and turn on physics rendering and set a high update rate
		physicsScene->SetState(Scene::State::Edit);
		physicsScene->SetRefreshRate(300);
	}

	void OnDetach() override
	{
		// do nothing
	}

	void OnUpdate(AEngine::TimeStep ts) override
	{
		using namespace AEngine;

		if (Application::Instance().GetInput().GetMouseButton(AEMouse::BUTTON_LEFT) == AEInputState::Pressed)
		{
			if (SceneManager::GetActiveScene()->GetState() == Scene::State::Simulate)
			{
				// get the position and direction of the camera
				// this will be used to orientate the projectile
				Math::vec3 front = Scene::GetDebugCamera().GetFront();
				Math::vec3 pos = Scene::GetDebugCamera().GetPosition();
				FireProjectile(pos, front);
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
		SetLayer(std::make_unique<DemoLayer>("Demo Layer"));

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
	props.title = "HAC Interactive Demo";
	return new DemoApp(props);
}
