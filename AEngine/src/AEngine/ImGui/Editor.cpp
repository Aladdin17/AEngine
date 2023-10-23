#include "Editor.h"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "AEngine/Resource/AssetManager.h"
#include "AEngine/Core/Application.h"
#include "AEngine/Events/EventHandler.h"
#include "AEngine/Events/KeyEvent.h"
#include "AEngine/Events/MouseEvent.h"

#include "AEngine/Scene/Entity.h"
#include "AEngine/Scene/Components.h"
#include "AEngine/Scene/SceneManager.h"
#include "AEngine/Core/Logger.h"

#include <iostream>

namespace AEngine
{
	void Editor::ShowMat3(const char* label, Math::mat3 & matrix)
	{
		ImGui::Text("%s", label);
		ImGui::PushID("##Inertia Tensor");
		ImGui::InputFloat3("I_x", &matrix[0][0], "%.3f");
		ImGui::InputFloat3("I_y", &matrix[1][0], "%.3f");
		ImGui::InputFloat3("I_z", &matrix[2][0], "%.3f");
		ImGui::PopID();
	}

	void Editor::Init(Window *window, const EditorProperties& props)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();

		// How to deal with mouse input; arguably don't need to unset
		// the keyboard (no option) as it won't be polled without the mouse hover
		// -> io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		// Maybe add a boolean for the `viewport capturing` input, which can
		// be used to determine how this setting is set/unset

		io.ConfigWindowsMoveFromTitleBarOnly = props.TitleBarMove;
		if(props.IsDockingEnabled)
		{
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}

		if(props.IsViewportEnabled)
		{
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		}

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->GetNative()), true);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::StyleColorsDark();

		ImGuiStyle& guiStyle = ImGui::GetStyle();
		guiStyle.WindowMenuButtonPosition = ImGuiDir_None;
		guiStyle.Alpha = 0.8f;

		// Handle events from the window and pass to game layer if needed
		window->RegisterEventHandler<KeyPressed>(0, [&io, this](KeyPressed& e) -> bool {
			if (Application::Instance().isEditMode())
			{
				return false;
			}

			return ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse;
		});
		window->RegisterEventHandler<KeyReleased>(0, [this](KeyReleased& e) -> bool {
			if (Application::Instance().isEditMode())
			{
				return false;
			}

			return ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse;
		});

		window->RegisterEventHandler<MouseButtonPressed>(0, [this](MouseButtonPressed& e) -> bool {
			if (Application::Instance().isEditMode())
			{
				return false;
			}

			return ImGui::GetIO().WantCaptureMouse;
		});
		window->RegisterEventHandler<MouseButtonReleased>(0, [this](MouseButtonReleased& e) -> bool {
			if (Application::Instance().isEditMode())
			{
				return false;
			}

			return ImGui::GetIO().WantCaptureMouse;
		});
		window->RegisterEventHandler<MouseMoved>(0, [this](MouseMoved& e) -> bool {
			if (Application::Instance().isEditMode())
			{
				return false;
			}

			return ImGui::GetIO().WantCaptureMouse;
		});
	}

	void Editor::CreateNewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::Update()
	{
		/**
		 * \todo Think about implementing observer pattern to send out updates
		 * when the scene is changed. This would need to be added to the
		 * SceneManager class.
		*/
		m_scene = SceneManager::GetActiveScene();
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		// ShowGameViewPort();
		ShowHierarchy();
		ShowInspector();
		ShowDebugWindow();
		ShowDebugCameraConfig();

		// ImGui::End();
	}

	void Editor::Render()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO &io = ImGui::GetIO();
		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			//get our current window something like
			Window* window = Application::Instance().GetWindow();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			//change back to our window rendering context?
			window->MakeCurrent();
		}
	}

	void Editor::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::ShowDebugCameraConfig()
	{
		// Get attributes
		DebugCamera& debugCam = Scene::GetDebugCamera();
		// Perspective camera
		float fov = debugCam.GetFov();
		float aspect = debugCam.GetAspect();
		float nearPlane = debugCam.GetNearPlane();
		float farPlane = debugCam.GetFarPlane();
		// Debug camera
		Math::vec3 pos = debugCam.GetPosition();
		float movementSpeed = debugCam.GetMovementSpeed();
		float lookSensitivity = debugCam.GetLookSensitivity();

		ImGui::Begin("Editor Camera Config");
		ImGui::Text("Position: (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
		ImGui::Separator();
		ImGui::Text("Camera");
		ImGui::SliderFloat("FOV", &fov, 10.0f, 180.0f, "%.3f");
		ImGui::SliderFloat("Aspect", &aspect, 0.1f, 10.0f, "%.3f");
		ImGui::SliderFloat("Near Plane", &nearPlane, 0.1f, 10.0f, "%.3f");
		ImGui::SliderFloat("Far Plane", &farPlane, 10.0f, 10000.0f, "%.3f");
		ImGui::Separator();
		ImGui::Text("Movement");
		ImGui::SliderFloat("Movement Speed", &movementSpeed, 1.0f, 100.0f, "%.3f");
		ImGui::SliderFloat("Look Sensitivity", &lookSensitivity, 0.1f, 10.0f, "%.3f");
		ImGui::End();

		// Set attributes
		debugCam.SetFov(fov);
		debugCam.SetAspect(aspect);
		debugCam.SetNearPlane(nearPlane);
		debugCam.SetFarPlane(farPlane);
		debugCam.SetPosition(pos);
		debugCam.SetMovementSpeed(movementSpeed);
		debugCam.SetLookSensitivity(lookSensitivity);
	}

	void Editor::ShowDebugWindow()
	{
		ImGui::Begin("Editor Debug");
		// group of radio buttons for scene state
		ImGui::Text("Select Scene State");
		ImGui::Spacing();
		int sceneState = static_cast<int>(m_scene->GetState());
		ImGui::RadioButton("Edit", &sceneState, static_cast<int>(Scene::State::Edit));
		ImGui::SameLine();
		ImGui::RadioButton("Simulate", &sceneState, static_cast<int>(Scene::State::Simulate));
		ImGui::SameLine();
		ImGui::RadioButton("Pause", &sceneState, static_cast<int>(Scene::State::Pause));
		m_scene->SetState(static_cast<Scene::State>(sceneState));
		ImGui::Separator();

		// physics simulation rates
		int physicsUpdateRate = m_scene->GetPhysicsUpdateRate();
		float timeScale = m_scene->GetTimeScale();
		ImGui::SliderInt("Physics Update Rate", &physicsUpdateRate, 1, 1200);
		ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 2.0f, "%.3f");
		m_scene->SetPhysicsUpdateRate(physicsUpdateRate);
		m_scene->SetTimeScale(timeScale);
		ImGui::Separator();

		// step the simulation
		if (m_scene->GetState() == Scene::State::Pause)
		{
			if (ImGui::Button("Advance One Simulation Step"))
			{
				m_scene->AdvanceOneSimulationStep();
			}
		}

		ImGui::End();
	}

	void Editor::ShowHierarchy()
	{
		constexpr float removeButtonWidth = 20.0f;
		constexpr float removeButtonPadding = 10.0f;

		std::vector<Uint16> entityids;
		m_scene->GetEntityIds(entityids);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImGui::Begin(std::string("Hierarchy " + m_scene->GetIdent()).c_str());
		if (ImGui::Button("Save Scene"))
		{
			SceneManager::SaveActiveToFile("serialized.scene");
		}
		if (ImGui::BeginPopup("Add Entity Popup"))
		{
			bool accept = false;
			static char name[32] = "Entity";
			ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
			if (ImGui::Button("Add", ImVec2(120, 0)))
			{
				accept = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				accept = false;
				ImGui::CloseCurrentPopup();
			}

			if (accept)
			{
				// Add entity to scene, with a default transform component
				Entity newEntity = m_scene->CreateEntity(name);
				newEntity.AddComponent<TransformComponent>();
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Add Entity"))
		{
			ImGui::OpenPopup("Add Entity Popup");
		}

		for(int i = 0; i < entityids.size(); i++)
		{
			Entity entity = m_scene->GetEntity(entityids[i]);
			std::string entityName = entity.GetComponent<TagComponent>()->tag;
			if (entity != m_selectedEntity)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
				const float buttonWidth = ImGui::GetContentRegionAvail().x - (removeButtonWidth + removeButtonPadding);
				if(ImGui::Button(entityName.c_str(), ImVec2(buttonWidth, 0)))
				{
					m_selectedEntity = m_scene->GetEntity(entityids[i]);
				}
				ImGui::PopStyleVar();
			}
			else
			{
				// Is selected entity, so just replace with coloured text
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text(entityName.c_str());
				ImGui::PopStyleColor();
			}

			// Add a remove button
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.0f, 0.0f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 0.6f));
			ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - (removeButtonWidth + removeButtonPadding));
			if (ImGui::Button(std::string("X##" + entityName).c_str(), ImVec2(20, 0)))
			{
				// remove entity and reset selected entity
				m_scene->RemoveEntity(entityName);
				m_selectedEntity = Entity();
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::End();

		ImGui::PopStyleColor(3);
	}

	void Editor::ShowInspector()
	{
		ImGui::Begin("Inspector");

		// No entity selected message
		if (!m_selectedEntity.IsValid())
		{
			ImGui::Text("No entity selected");
			ImGui::End();
			return;
		}

		ShowAddComponentButton();
		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("Add Component");
		}

		ImGui::Separator();
		ImGui::Spacing();

		ShowTagComponent();
		ShowTransformComponent();
		ShowRenderableComponent();
		ShowSkinnedRenderableComponent();
		ShowSkyboxComponent();
		ShowCameraComponent();
		ShowScriptableComponent();
		ShowRigidBodyComponent();
		ShowCollisionBodyComponent();
		ShowPlayerControllerComponent();
		ShowRectTransformComponent();
		ShowCanvasRendererComponent();
		ShowPanelComponent();
		ShowTextComponent();

		//might need to have similar function for each entity and check against entity to view everything
		//build out for all components

		ImGui::End();
	}

	void Editor::ShowAddComponentButton()
	{
		assert (m_selectedEntity.IsValid());

		if (ImGui::BeginPopup("Add Component"))
		{
			if (!m_selectedEntity.HasComponent<RigidBodyComponent>())
			{
				ShowAddComponentPrompt<CollisionBodyComponent>("Collision Body");
			}

			if (!m_selectedEntity.HasComponent<CollisionBodyComponent>())
			{
				ShowAddComponentPrompt<RigidBodyComponent>("Rigid Body");
			}

			ShowAddComponentPrompt<RenderableComponent>("Renderable");
			ImGui::EndPopup();
		}
	}

	template <typename T>
	void Editor::ShowAddComponentPrompt(const char* label)
	{
		if (!m_selectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(label))
			{
				m_selectedEntity.AddComponent<T>();
			}
		}
	}

	void Editor::ShowTagComponent()
	{
		TagComponent* tc = m_selectedEntity.GetComponent<TagComponent>();
		if(tc != nullptr)
		{
			ImGui::Text("Name: %s", tc->tag.c_str());
			ImGui::Text("ID: %d", tc->ident);
		}
	}

	void Editor::ShowTransformComponent()
	{
		TransformComponent* tc = m_selectedEntity.GetComponent<TransformComponent>();
		if(tc != nullptr)
		{
			if(ImGui::CollapsingHeader("Transform Component"))
			{
				Math::vec3* translation =  &tc->translation;
				ImGui::DragFloat3("Translation", &(translation->x), 1.0f, 0.0f, 0.0f, "%.3f");

				Math::quat* orientation = &tc->orientation;
        		Math::vec3 eulerAnglesDegrees = Math::degrees(Math::eulerAngles(*orientation));
        		ImGui::DragFloat3("Rotation", &eulerAnglesDegrees.x, 1.0f, 0.0f, 0.0f, "%.3f");
        		*orientation = Math::quat(Math::radians(eulerAnglesDegrees));

				Math::vec3* scale = &tc->scale;
				ImGui::DragFloat3("Scale", &(scale->x), 1.0f, 0.0f, FLT_MAX, "%.3f");
			}
		}
	}

	void Editor::ShowRenderableComponent()
	{
		RenderableComponent* rc = m_selectedEntity.GetComponent<RenderableComponent>();
		if(rc != nullptr)
		{
			if (ImGui::BeginPopup("Model Selection"))
			{
				// show all models in asset manager
				std::map<std::string, SharedPtr<Model>>::const_iterator it;
				for (it = AssetManager<Model>::Instance().begin(); it != AssetManager<Model>::Instance().end(); ++it)
				{
					if (ImGui::MenuItem(it->first.c_str()))
					{
						rc->model = it->second;
					}
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("Shader Selection"))
			{
				// show all models in asset manager
				std::map<std::string, SharedPtr<Shader>>::const_iterator it;
				for (it = AssetManager<Shader>::Instance().begin(); it != AssetManager<Shader>::Instance().end(); ++it)
				{
					if (ImGui::MenuItem(it->first.c_str()))
					{
						rc->shader = it->second;
					}
				}
				ImGui::EndPopup();
			}

			Model* model = rc->model.get();
			Shader* shader = rc->shader.get();
			if(ImGui::CollapsingHeader("Renderable Component"))
			{
				const std::string& modelIdent = model ? model->GetIdent() : "None";
				const std::string& shaderIdent = shader ? shader->GetIdent() : "None";

				ImGui::Checkbox("IsActive", &(rc->active));
				ImGui::Text("Model");
				ImGui::SameLine();
				if (ImGui::Button(modelIdent.c_str()))
				{
					ImGui::OpenPopup("Model Selection");
				}

				ImGui::Text("Shader");
				ImGui::SameLine();
				if (ImGui::Button(shaderIdent.c_str()))
				{
					ImGui::OpenPopup("Shader Selection");
				}
			}
		}
	}

	void Editor::ShowSkinnedRenderableComponent()
	{
		SkinnedRenderableComponent* src = m_selectedEntity.GetComponent<SkinnedRenderableComponent>();
		if(src != nullptr)
		{
			if(ImGui::CollapsingHeader("Skinned Renderable Component"))
			{
				ImGui::Checkbox("IsActive", &(src->active));
				//model and shader and animator
			}
		}
	}

	void Editor::ShowSkyboxComponent()
	{
		SkyboxComponent* sb = m_selectedEntity.GetComponent<SkyboxComponent>();
		if(sb != nullptr)
		{
			if(ImGui::CollapsingHeader("SkyBox Component"))
			{
				ImGui::Checkbox("Is Active", &(sb->active));
				//skybox
				//shader
			}
		}
	}

	void Editor::ShowCameraComponent()
	{
		CameraComponent* cc = m_selectedEntity.GetComponent<CameraComponent>();
		if(cc != nullptr)
		{
			if(ImGui::CollapsingHeader("Camera Component"))
			{
				//Perspective Camera
			}
		}
	}

	void Editor::ShowScriptableComponent()
	{
		ScriptableComponent* sc = m_selectedEntity.GetComponent<ScriptableComponent>();
		if(sc != nullptr)
		{
			if(ImGui::CollapsingHeader("Scriptable Component"))
			{
				//script
			}
		}
	}

	void Editor::ShowCollisionBodyComponent()
	{
		CollisionBodyComponent* cc = m_selectedEntity.GetComponent<CollisionBodyComponent>();
		if(cc != nullptr)
		{
			TransformComponent* tc = m_selectedEntity.GetComponent<TransformComponent>();

			/// \todo Rework the way collision bodies are created in the editor
			/// For now, create a new collision body if one doesn't exist
			if (!cc->ptr)
			{
				PhysicsWorld* world = m_scene->GetPhysicsWorld();
				cc->ptr= world->AddCollisionBody(tc->translation, tc->orientation);
			}

			if(ImGui::CollapsingHeader("CollisionBody Component"))
			{
				// Add collider popup
				if (ImGui::BeginPopup("Add Collider Popup##CollisionBody"))
				{
					if (ImGui::MenuItem("Box Collider"))
					{
						cc->ptr->AddBoxCollider(Math::vec3(1.0f, 1.0f, 1.0f), Math::vec3(0.0f, 0.0f, 0.0f), tc->orientation);
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				// Add collider button
				if (ImGui::Button("Add Collider##CollisionBody"))
				{
					ImGui::OpenPopup("Add Collider Popup##CollisionBody");
				}

				// list the colliders
				CollisionBody* cb = cc->ptr.get();
				UniquePtr<Collider> collider = cb->GetCollider();
				if (collider)
				{
					bool isTrigger = collider->GetIsTrigger();
					ImGui::Text("Colliders");
					ImGui::Separator();
					ImGui::Text("Type: %s", collider ? collider->GetName() : "None");
					ImGui::Checkbox("Is Trigger", &isTrigger);
					collider->SetIsTrigger(isTrigger);

					// General configurations
					Math::vec3 offset = collider->GetOffset();
					Math::vec3 orientation = Math::degrees(Math::eulerAngles(collider->GetOrientation()));
					ImGui::DragFloat3("Offset", &offset.x, 0.1f, 0.0f, 0.0f, "%.3f");
					ImGui::DragFloat3("Orientation", &orientation.x, 0.1f, 0.0f, 0.0f, "%.3f");
					collider->SetOffset(offset);
					collider->SetOrientation(Math::quat(Math::radians(orientation)));
					ImGui::Separator();
					// Box collider configurations
					if (collider->GetType() == Collider::Type::Box)
					{
						Math::vec3 size = dynamic_cast<BoxCollider*>(collider.get())->GetSize();
						ImGui::DragFloat3("Size", &size.x, 0.1f, 0.0f, FLT_MAX, "%.3f");
						dynamic_cast<BoxCollider*>(collider.get())->Resize(size);
					}
				}
			}
		}
	}


	void Editor::ShowRigidBodyComponent()
	{
		RigidBodyComponent* rc = m_selectedEntity.GetComponent<RigidBodyComponent>();
		if(rc != nullptr)
		{
			TransformComponent* tc = m_selectedEntity.GetComponent<TransformComponent>();

			/// \todo Rework the way collision bodies are created in the editor
			/// For now, create a new collision body if one doesn't exist
			if (!rc->ptr)
			{
				PhysicsWorld* world = m_scene->GetPhysicsWorld();
				rc->ptr = world->AddRigidBody(tc->translation, tc->orientation);
			}

			if(ImGui::CollapsingHeader("RigidgBody Component"))
			{
				RigidBody* body = rc->ptr.get();

				// Set type
				ImGui::Text("Type");
				ImGui::SameLine();
				int rbType = static_cast<int>(body->GetType());
				ImGui::RadioButton("Dynamic", &rbType, static_cast<int>(RigidBody::Type::Dynamic));
				ImGui::SameLine();
				ImGui::RadioButton("Static", &rbType, static_cast<int>(RigidBody::Type::Static));
				ImGui::SameLine();
				ImGui::RadioButton("Kinematic", &rbType, static_cast<int>(RigidBody::Type::Kinematic));
				body->SetType(static_cast<RigidBody::Type>(rbType));
				ImGui::Spacing();
				ImGui::Spacing();

				// Properties
				bool hasGravity = body->GetHasGravity();
				float mass = body->GetMass();
				float linearDamping = body->GetLinearDamping();
				float angularDamping = body->GetAngularDamping();
				ImGui::Checkbox("Has Gravity", &hasGravity);
				body->SetHasGravity(hasGravity);
				ImGui::InputFloat("Mass", &mass);
				if (mass > 0.0f)
				{
					body->SetMass(mass);
				}
				ImGui::DragFloat("Linear Damping", &linearDamping, 0.01f, 0.0f, 1.0f, "%.3f");
				body->SetLinearDamping(linearDamping);
				ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f, 0.0f, 1.0f, "%.3f");
				body->SetAngularDamping(angularDamping);
				ImGui::Spacing();
				ImGui::Spacing();

				// inertia tensor
				Math::mat3 inertiaTensor = body->GetInertiaTensor();
				ShowMat3("Inertia Tensor", inertiaTensor);
				body->SetInertiaTensor(inertiaTensor);
				ImGui::Spacing();
				ImGui::Spacing();

				// velocities
				Math::vec3 linearVelocity = body->GetLinearVelocity();
				Math::vec3 angularVelocity = body->GetAngularVelocity();
				ImGui::DragFloat3("Linear Velocity", &linearVelocity.x, 0.1f, 0.0f, 0.0f, "%.3f");
				ImGui::SameLine();
				ImGui::Text("(%.3f m/s)", Math::length(linearVelocity));
				ImGui::DragFloat3("Angular Velocity", &angularVelocity.x, 0.1f, 0.0f, 0.0f, "%.3f");
				ImGui::Text("DPS/RPM");
				ImGui::SameLine();
				Math::vec3 dps = Math::degrees(angularVelocity);
				Math::vec3 rpm = dps / 6.0f;
				ImGui::Text("x = (%.3f / %.3f), y = (%.3f / %.3f), z = (%.3f / %.3f)", dps.x, rpm.x, dps.y, rpm.y, dps.z, rpm.z);

				body->SetLinearVelocity(linearVelocity);
				body->SetAngularVelocity(angularVelocity);
				ImGui::Spacing();
				ImGui::Spacing();


				ImGui::Text("Colliders");
				ImGui::Separator();

				// Add collider popup
				if (ImGui::BeginPopup("Add Collider Popup##RigidBody"))
				{
					if (ImGui::MenuItem("Box Collider"))
					{
						body->AddBoxCollider(Math::vec3(1.0f, 1.0f, 1.0f), Math::vec3(0.0f, 0.0f, 0.0f), tc->orientation);
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				// Add collider button
				if (ImGui::Button("Add Collider##RigidBody"))
				{
					ImGui::OpenPopup("Add Collider Popup##RigidBody");
				}

				// list the colliders
				UniquePtr<Collider> collider = body->GetCollider();
				if (collider)
				{
					bool isTrigger = collider->GetIsTrigger();
					ImGui::Text("Type: %s", collider ? collider->GetName() : "None");
					ImGui::Checkbox("Is Trigger", &isTrigger);
					collider->SetIsTrigger(isTrigger);

					// General configurations
					Math::vec3 offset = collider->GetOffset();
					Math::vec3 orientation = Math::degrees(Math::eulerAngles(collider->GetOrientation()));
					ImGui::DragFloat3("Offset", &offset.x, 0.1f, 0.0f, 0.0f, "%.3f");
					ImGui::DragFloat3("Orientation", &orientation.x, 0.1f, 0.0f, 0.0f, "%.3f");
					collider->SetOffset(offset);
					collider->SetOrientation(Math::quat(Math::radians(orientation)));
					ImGui::Separator();
					// Box collider configurations
					if (collider->GetType() == Collider::Type::Box)
					{
						Math::vec3 size = dynamic_cast<BoxCollider*>(collider.get())->GetSize();
						ImGui::DragFloat3("Size", &size.x, 0.1f, 0.0f, FLT_MAX, "%.3f");
						dynamic_cast<BoxCollider*>(collider.get())->Resize(size);
					}
				}
			}
		}
	}

	void Editor::ShowPlayerControllerComponent()
	{
		PlayerControllerComponent* pcc = m_selectedEntity.GetComponent<PlayerControllerComponent>();
		if(pcc != nullptr)
		{
			if(ImGui::CollapsingHeader("Player Controller Component"))
			{
				ImGui::InputFloat("Radius: ", &(pcc->radius), 0.01f, 0.1f, "%.3f");
				ImGui::InputFloat("Height: ", &(pcc->height), 0.01f, 0.1f, "%.3f");
				ImGui::InputFloat("Speed: ", &(pcc->speed), 0.01f, 0.1f, "%.3f");
				ImGui::InputFloat("Move Drag: ", &(pcc->moveDrag), 0.01f, 0.1f, "%.3f");
				ImGui::InputFloat("Fall Drag: ", &(pcc->fallDrag), 0.01f, 0.1f, "%.3f");
			}
		}
	}

	void Editor::ShowRectTransformComponent()
	{
		RectTransformComponent* rtc = m_selectedEntity.GetComponent<RectTransformComponent>();
		if(rtc != nullptr)
		{
			if(ImGui::CollapsingHeader("Rect Transform Component"))
			{
				Math::vec3* translation =  &rtc->translation;
				ImGui::DragFloat2("Translation", &(translation->x), 0.001f, 0.0f, 0.0f, "%.4f");

				Math::quat* orientation = &rtc->orientation;
        		Math::vec3 eulerAnglesDegrees = Math::degrees(Math::eulerAngles(*orientation));
        		ImGui::DragFloat2("Rotation", &eulerAnglesDegrees.x, 0.001f, 0.0f, 0.0f, "%.4f");
        		*orientation = Math::quat(Math::radians(eulerAnglesDegrees));

				Math::vec3* scale = &rtc->scale;
				ImGui::DragFloat2("Scale", &(scale->x), 0.1f, 0.0f, FLT_MAX, "%.3f");

				Math::vec2* size = &rtc->size;
				ImGui::DragFloat2("Size", &(size->x), 1.0f, 0.0f, FLT_MAX, "%.3f");
			}
		}
	}

	void Editor::ShowCanvasRendererComponent()
	{
		CanvasRenderer* crc = m_selectedEntity.GetComponent<CanvasRenderer>();
		if(crc != nullptr)
		{
			if(ImGui::CollapsingHeader("Canvas Renderer Component"))
			{
				bool* active = &crc->active;
				ImGui::Checkbox("IsActive", active);

				bool* screenspace = &crc->screenSpace;
				ImGui::Checkbox("Screen Space", screenspace);
			}
		}
	}

	void Editor::ShowPanelComponent()
	{
		PanelComponent* pc = m_selectedEntity.GetComponent<PanelComponent>();
		if(pc != nullptr)
		{

			if (ImGui::BeginPopup("Texture Selection"))
			{
				// show all models in asset manager
				std::map<std::string, SharedPtr<Texture>>::const_iterator it;
				for (it = AssetManager<Texture>::Instance().begin(); it != AssetManager<Texture>::Instance().end(); ++it)
				{
					if (ImGui::MenuItem(it->first.c_str()))
					{
						pc->texture = it->second;
					}
				}
				ImGui::EndPopup();
			}

			Texture* texture = pc->texture.get();

			if(ImGui::CollapsingHeader("Panel Component"))
			{
				const std::string& textureIdent = texture ? texture->GetIdent() : "None";

				Math::vec4* color = &pc->color;
				ImGui::ColorEdit4("Color", &(color->x));

				ImGui::Text("Texture");
				ImGui::SameLine();
				if (ImGui::Button(textureIdent.c_str()))
				{
					ImGui::OpenPopup("Texture Selection");
				}
			}
		}
	}

	void Editor::ShowTextComponent()
	{
		TextComponent* tc = m_selectedEntity.GetComponent<TextComponent>();
		if(tc != nullptr)
		{
			if (ImGui::BeginPopup("Font Selection"))
			{
				// show all fonts in asset manager
				std::map<std::string, SharedPtr<Font>>::const_iterator it;
				for (it = AssetManager<Font>::Instance().begin(); it != AssetManager<Font>::Instance().end(); ++it)
				{
					if (ImGui::MenuItem(it->first.c_str()))
					{
						tc->font = it->second;
					}
				}
				ImGui::EndPopup();
			}

			Font* font = tc->font.get();

			if(ImGui::CollapsingHeader("Text Component"))
			{
				const std::string& fontIdent = font ? font->GetIdent() : "None";

				Math::vec4* color = &tc->color;
				ImGui::ColorEdit4("Color", &(color->x));

				ImGui::Text("Font");
				ImGui::SameLine();
				if (ImGui::Button(fontIdent.c_str()))
				{
					ImGui::OpenPopup("Font Selection");
				}

				std::string* text = &tc->text;
				ImGui::InputText("Text", text->data(), text->size());
			}
		}
	}
}
