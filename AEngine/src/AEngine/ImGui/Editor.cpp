#include "Editor.h"
#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "AEngine/Core/Types.h"
#include "AEngine/Resource/AssetManager.h"
#include "AEngine/Core/Application.h"
#include "AEngine/Events/EventHandler.h"
#include "AEngine/Events/KeyEvent.h"
#include "AEngine/Events/MouseEvent.h"

#include "AEngine/Scene/Entity.h"
#include "AEngine/Scene/Components.h"
#include "AEngine/Scene/SceneManager.h"
#include "AEngine/Core/Logger.h"

#include "AEngine/Physics/Collider.h"
#include "AEngine/Physics/CollisionBody.h"
#include "AEngine/Physics/PlayerController.h"

#include <iostream>
#include <string>

namespace AEngine
{
	void Editor::Init(Window *window, const EditorProperties& props)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();

		m_guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		m_guizmoMode = ImGuizmo::MODE::WORLD;

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
		guiStyle.FrameRounding = 4.0f;
		guiStyle.WindowRounding = 4.0f;
		guiStyle.GrabRounding = 2.0f;


		// Handle events from the window, populating input buffer and propagating if needed
		window->RegisterEventHandler<KeyReleased>(1, [this](KeyReleased& e) -> bool {
			m_input.SetKeyState(e.GetKey(), AEInputState::Released);
			return m_handleInput;
		});
		window->RegisterEventHandler<KeyPressed>(1, [this](KeyPressed& e) -> bool {
			m_input.SetKeyState(e.GetKey(), AEInputState::Pressed);
			return m_handleInput;
		});
		window->RegisterEventHandler<KeyRepeated>(1, [this](KeyRepeated& e) -> bool {
			m_input.SetKeyState(e.GetKey(), AEInputState::Repeated);
			return m_handleInput;
		});
		window->RegisterEventHandler<MouseReleased>(1, [this](MouseReleased& e) -> bool {
			m_input.SetMouseButtonState(e.GetButton(), AEInputState::Released);
			return m_handleInput;
		});
		window->RegisterEventHandler<MousePressed>(1, [this](MousePressed& e) -> bool {
			m_input.SetMouseButtonState(e.GetButton(), AEInputState::Pressed);
			return m_handleInput;
		});
		window->RegisterEventHandler<MouseRepeated>(1, [this](MouseRepeated& e) -> bool {
			m_input.SetMouseButtonState(e.GetButton(), AEInputState::Repeated);
			return m_handleInput;
		});
		window->RegisterEventHandler<MouseMoved>(1, [this](MouseMoved& e) -> bool {
			m_input.SetMouseState(e.GetPos(), e.GetDelta());
			return m_handleInput;
		});
		window->RegisterEventHandler<MouseScrolled>(1, [this](MouseScrolled& e) -> bool {
			m_input.SetMouseScroll(e.GetScroll());
			return m_handleInput;
		});
	}

	void Editor::CreateNewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
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


		// always check the editor toggle button
		AEInputState state = m_input.GetKey(AEKey::GRAVE_ACCENT);
		if (state == AEInputState::Pressed)
		{
			m_showEditor = !m_showEditor;
		}

		// handle the passthrough of input to the application
		if (m_input.IsMouseButtonDown(AEMouse::BUTTON_RIGHT))
		{
			m_handleInput = false;
			Application::Instance().GetWindow()->ShowCursor(false);
		}
		else
		{
			Application::Instance().GetWindow()->ShowCursor(true);
			m_handleInput = true;
		}

		if (m_handleInput)
		{
			// reset the input buffer if we break away into the editor
			Application::Instance().GetInput().Reset();
			HandleGeneralInput();
		}
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			io.WantCaptureKeyboard = false;
			io.WantCaptureMouse = false;
			ControlDebugCamera();
		}

		if (m_showEditor)
		{
			ShowHierarchy();
			ShowInspector();
			ShowDebugWindow();
			ShowGuizmos();
		}
	}

	void Editor::ControlDebugCamera()
	{
		// update the debug camera
		if (m_useDebugCamera)
		{
			m_scene->UseDebugCamera(true);
			DebugCamera& debugCam = m_scene->GetDebugCamera();
			Uint8 direction = 0;

			if (m_input.IsKeyDown(AEKey::W))
			{
				direction |= DebugCamera::Direction::Forward;
			}
			if (m_input.IsKeyDown(AEKey::A))
			{
				direction |= DebugCamera::Direction::Left;
			}
			if (m_input.IsKeyDown(AEKey::S))
			{
				direction |= DebugCamera::Direction::Backward;
			}
			if (m_input.IsKeyDown(AEKey::D))
			{
				direction |= DebugCamera::Direction::Right;
			}
			if (m_input.IsKeyDown(AEKey::SPACE))
			{
				direction |= DebugCamera::Direction::Up;
			}
			if (m_input.IsKeyDown(AEKey::LEFT_SHIFT))
			{
				direction |= DebugCamera::Direction::Down;
			}

			debugCam.SetMovement(direction);
			debugCam.SetRotation(m_input.GetMouseDelta());
		}
		else
		{
			m_scene->UseDebugCamera(false);
		}
	}

	void Editor::ShowGuizmos()
	{
		// return if there are no selected entities or if the guizmos are not shown
		if (!m_selectedEntity.IsValid() || !m_showGuizmos)
		{
			return;
		}


		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList(ImGui::GetMainViewport()));
		ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

		// get the debug camera
		DebugCamera& debugCam = m_scene->GetDebugCamera();
		const Math::mat4 &proj = debugCam.GetProjectionMatrix();
		Math::mat4 view = debugCam.GetViewMatrix();

		// get the transform for the selected entity
		TransformComponent* tc = m_selectedEntity.GetComponent<TransformComponent>();
		Math::mat4 transform = tc->ToMat4();


		// get the snap value
		float snapValue = 0.0f;
		if (m_guizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
		{
			snapValue = m_guizmoTranslateSnapInterval;
		}
		else if (m_guizmoOperation == ImGuizmo::OPERATION::ROTATE)
		{
			snapValue = m_guizmoRotateSnapInterval;
		}
		else if (m_guizmoOperation == ImGuizmo::OPERATION::SCALE)
		{
			snapValue = m_guizmoScaleSnapInterval;
		}

		// generate the snap values
		float snapValues[3] = { snapValue, snapValue, snapValue };
		// bool shouldSnap =  m_input.GetKey(AEKey::LEFT_CONTROL) == AEInputState::Repeated;
		ImGuizmo::Manipulate(
			Math::value_ptr(view),
			Math::value_ptr(proj),
			static_cast<ImGuizmo::OPERATION>(m_guizmoOperation),
			static_cast<ImGuizmo::MODE>(m_guizmoMode),
			Math::value_ptr(transform),
			nullptr//,
			// shouldSnap ? snapValues : nullptr
		);

		if (ImGuizmo::IsUsing())
		{
			Math::vec3 translation, rotation, scale;
			ImGuizmo::DecomposeMatrixToComponents(
				Math::value_ptr(transform),
				Math::value_ptr(translation),
				Math::value_ptr(rotation),
				Math::value_ptr(scale)
			);

			// convert the rotation to a quaternion
			Math::quat quatRotation = Math::quat(Math::radians(rotation));
			tc->translation = translation;
			tc->orientation = quatRotation;
			tc->scale = scale;
		}
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

	void Editor::CameraPanel(PerspectiveCamera* camera)
	{
		if (!camera)
		{
			return;
		}

		if (ImGui::TreeNode("Perspective Camera"))
		{
			float fov = camera->GetFov();
			float aspect = camera->GetAspect();
			float nearPlane = camera->GetNearPlane();
			float farPlane = camera->GetFarPlane();

			if (ImGui::SliderFloat("FOV", &fov, 10.0f, 180.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			{
				camera->SetFov(fov);
			}
			if (ImGui::SliderFloat("Aspect", &aspect, 0.1f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			{
				camera->SetAspect(aspect);
			}
			if (ImGui::SliderFloat("Near Plane", &nearPlane, 0.001f, 10000.0f, "%.3f"))
			{
				camera->SetNearPlane(nearPlane);
			}
			if (ImGui::SliderFloat("Far Plane", &farPlane, 0.001f, 10000.0f, "%.3f"))
			{
				camera->SetFarPlane(farPlane);
			}

			ImGui::TreePop();
		}
	}

	void Editor::ShowEditor(bool show)
	{
		m_showEditor = show;
	}

	void Editor::HandleGeneralInput()
	{
		// handle the guizmo input
		if (!ImGuizmo::IsUsing())
		{
			if (m_input.GetKey(AEKey::Q) == AEInputState::Pressed)
			{
				if (m_input.IsKeyDown(AEKey::LEFT_ALT))
				{
					m_showGuizmos = !m_showGuizmos;
				}
				else
				{
					// swap the guizmo mode
					if (m_guizmoMode == ImGuizmo::MODE::LOCAL)
					{
						m_guizmoMode = ImGuizmo::MODE::WORLD;
					}
					else
					{
						m_guizmoMode = ImGuizmo::MODE::LOCAL;
					}
				}
			}
			if (m_input.GetKey(AEKey::T) == AEInputState::Pressed)
			{
				m_guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			}
			else if (m_input.GetKey(AEKey::R) == AEInputState::Pressed)
			{
				m_guizmoOperation = ImGuizmo::OPERATION::ROTATE;
			}
			else if (m_input.GetKey(AEKey::E) == AEInputState::Pressed)
			{
				m_guizmoOperation = ImGuizmo::OPERATION::SCALE;
			}
		}

		if (m_input.IsMouseButtonDown(AEMouse::BUTTON_RIGHT))
		{
			m_handleInput = false;
			Application::Instance().GetWindow()->ShowCursor(false);
		}
		else
		{
			Application::Instance().GetWindow()->ShowCursor(true);
			m_handleInput = true;
		}
	}

	void Editor::ShowDebugCameraConfig()
	{
		// Get attributes
		DebugCamera& debugCam = Scene::GetDebugCamera();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Checkbox("Use Debug Camera", &m_useDebugCamera);
		Math::vec3 pos = debugCam.GetPosition();
		float pitch = debugCam.GetPitch();
		float yaw = debugCam.GetYaw();
		float movementSpeed = debugCam.GetMovementSpeed();
		float lookSensitivity = debugCam.GetLookSensitivity();

		if (ImGui::DragFloat3("Position", &pos[0], 0.1f, 0.0f, 0.0f, "%.3f"))
		{
			debugCam.SetPosition(pos);
		}

		if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			debugCam.SetPitch(pitch);
		}
		if (ImGui::SliderFloat("Yaw", &yaw, 0.0f, 360.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			debugCam.SetYaw(yaw);
		}

		if (ImGui::SliderFloat("Movement Speed", &movementSpeed, 1.0f, 100.0f, "%.3f"))
		{
			debugCam.SetMovementSpeed(movementSpeed);
		}
		if (ImGui::SliderFloat("Look Sensitivity", &lookSensitivity, 0.1f, 10.0f, "%.3f"))
		{
			debugCam.SetLookSensitivity(lookSensitivity);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		// perspective camera config
		PerspectiveCamera* perspective = dynamic_cast<PerspectiveCamera*>(&debugCam);
		CameraPanel(perspective);
	}

	void Editor::ShowDebugWindow()
	{
		ImGui::Begin("Editor Debug");
		{
			if (ImGui::BeginTabBar("Editor Tab Bar"))
			{
				if (ImGui::BeginTabItem("Simulation"))
				{
					int sceneState = static_cast<int>(m_scene->GetState());
					// Show the current state of the scene
					ImGui::RadioButton("Edit", &sceneState, static_cast<int>(Scene::State::Edit));
					ImGui::SameLine();
					ImGui::RadioButton("Simulate", &sceneState, static_cast<int>(Scene::State::Simulate));
					ImGui::SameLine();
					ImGui::RadioButton("Pause", &sceneState, static_cast<int>(Scene::State::Pause));
					m_scene->SetState(static_cast<Scene::State>(sceneState));

					// step the simulation
					if (m_scene->GetState() == Scene::State::Pause)
					{
						ImGui::SameLine();
						if (ImGui::Button("Advance One Update Step"))
						{
							m_scene->AdvanceOneSimulationStep();
						}
					}
					ImGui::Spacing();
					ImGui::Spacing();

					// simulation rates
					int physicsUpdateRate = m_scene->GetRefreshRate();
					float timeScale = m_scene->GetTimeScale();
					if (ImGui::SliderInt("Update Rate", &physicsUpdateRate, 1, 6000, "%d Hz", ImGuiSliderFlags_AlwaysClamp))
					{
						m_scene->SetRefreshRate(physicsUpdateRate);
					}
					if (ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 2.0f, "%.3f x", ImGuiSliderFlags_AlwaysClamp))
					{
						m_scene->SetTimeScale(timeScale);
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Physics"))
				{
					PhysicsPanel();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Debug Camera"))
				{
					ShowDebugCameraConfig();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Guizmo"))
				{
					ImGui::Checkbox("Show Guizmos", &m_showGuizmos);
					ImGui::Text("Guizmo Mode");
					ImGui::RadioButton("Translate", &m_guizmoOperation, ImGuizmo::OPERATION::TRANSLATE);
					ImGui::SameLine();
					ImGui::RadioButton("Rotate", &m_guizmoOperation, ImGuizmo::OPERATION::ROTATE);
					ImGui::SameLine();
					ImGui::RadioButton("Scale", &m_guizmoOperation, ImGuizmo::OPERATION::SCALE);
					ImGui::Spacing();
					ImGui::Text("Coordinate Space");
					ImGui::RadioButton("Local", &m_guizmoMode, ImGuizmo::MODE::LOCAL);
					ImGui::SameLine();
					ImGui::RadioButton("World", &m_guizmoMode, ImGuizmo::MODE::WORLD);
					ImGui::Spacing();
					ImGui::Text("Snapping Intervals");
					ImGui::DragFloat("Translate##TranslateIncrement", &m_guizmoTranslateSnapInterval, 0.1f, 0.001f, 0.0f, "%.3f");
					ImGui::DragFloat("Rotate##RotateIncrement", &m_guizmoRotateSnapInterval, 0.1f, 0.001f, 45.0f, "%.3f");
					ImGui::DragFloat("Scale##ScaleIncrement", &m_guizmoScaleSnapInterval, 0.1f, 0.001f, 0.0f, "%.3f");

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}

		ImGui::End();
	}

	void Editor::ShowHierarchy()
	{
		std::vector<Uint16> entityIds;
		m_scene->GetEntityIds(entityIds);

		ImGui::Begin(std::string("Hierarchy (Scene : " + m_scene->GetIdent() + ")").c_str());
		{
			if (ImGui::Button("Save Scene"))
			{
				ImGui::OpenPopup("Save Scene Popup");
			}
			ImGui::SameLine();
			if (ImGui::Button("Add Entity"))
			{
				ImGui::OpenPopup("Add Entity Popup");
			}
			if (m_selectedEntity.IsValid())
			{
				ImGui::SameLine();
				if (ImGui::Button("Deselect"))
				{
					m_selectedEntity = Entity();
				}
			}

			if (ImGui::BeginPopup("Save Scene Popup"))
			{
				static char name[32] = "Test";
				ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

				if (ImGui::Button("Save", ImVec2(120, 0)))
				{
					SceneManager::SaveActiveToFile("assets/scenes/" + std::string(name) + ".scene");
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("Add Entity Popup"))
			{
				static char name[32] = "Entity";
				ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

				if (ImGui::Button("Add", ImVec2(120, 0)))
				{
					Entity newEntity = m_scene->CreateEntity(name);
					newEntity.AddComponent<TransformComponent>();
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			for (int i = 0; i < entityIds.size(); i++)
			{
				Entity entity = m_scene->GetEntity(entityIds[i]);
				std::string entityName = entity.GetComponent<TagComponent>()->tag;
				std::string uniqueID = entityName + "##" + std::to_string(i);

				bool isSelected = (entity == m_selectedEntity);
				ImGui::PushStyleColor(ImGuiCol_Text, isSelected ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

				if (ImGui::Selectable(uniqueID.c_str(), isSelected))
				{
					m_selectedEntity = entity;
				}

				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					// get the position of the entity
					Math::vec3 position = entity.GetComponent<TransformComponent>()->translation;

					// translate the debug camera to the position of the entity
					DebugCamera& debugCam = m_scene->GetDebugCamera();
					debugCam.SetPosition(position - Math::vec3(0.0f, 0.0f, 50.0f));
					debugCam.SetPitch(0.0f);
					debugCam.SetYaw(90.0f);
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Remove Entity"))
					{
						m_scene->RemoveEntity(entityName);
						m_selectedEntity = Entity();
					}

					ImGui::EndPopup();
				}
			}
		}

		ImGui::End();
	}

	void Editor::ShowInspector()
	{
		ImGui::Begin("Inspector");
		{
			// No entity selected message
			if (!m_selectedEntity.IsValid())
			{
				ImGui::Text("No entity selected");
				ImGui::End();
				return;
			}

			ShowTagComponent();
			ShowTransformComponent();
			ShowRenderableComponent();
			ShowSkinnedRenderableComponent();
			ShowSkyboxComponent();
			ShowCameraComponent();
			ShowScriptableComponent();
			ShowPlayerControllerComponent();
			ShowRectTransformComponent();
			ShowCanvasRendererComponent();
			ShowPanelComponent();
			ShowTextComponent();
			ShowNavigationComponent();
			ShowBDIComponent();
			ShowFCMComponent();

			// a little hacky for now
			if (m_selectedEntity.HasComponent<CollisionBodyComponent>())
			{
				// check that a collision body exists and if not create one
				CollisionBodyComponent* cbc = m_selectedEntity.GetComponent<CollisionBodyComponent>();
				if (!cbc->ptr)
				{
					TransformComponent* tc = m_selectedEntity.GetComponent<TransformComponent>();
					cbc->ptr = m_scene->GetPhysicsWorld()->AddCollisionBody(tc->translation, tc->orientation);
				}

				if (ImGui::CollapsingHeader("CollisionBody Component"))
				{
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Remove CollisionBody Component"))
						{
							m_selectedEntity.RemoveComponent<CollisionBodyComponent>();
						}

						ImGui::EndPopup();
					}
					CollisionBodyPanel(cbc->ptr.get());
				}
			}

			if (m_selectedEntity.HasComponent<RigidBodyComponent>())
			{
				// check that a rigidbody existts and if not create one
				RigidBodyComponent* rbc = m_selectedEntity.GetComponent<RigidBodyComponent>();
				if (!rbc->ptr)
				{
					TransformComponent* tc = m_selectedEntity.GetComponent<TransformComponent>();
					rbc->ptr = m_scene->GetPhysicsWorld()->AddRigidBody(tc->translation, tc->orientation);
				}

				if (ImGui::CollapsingHeader("RigidBody Component"))
				{
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Remove RigidBody Component"))
						{
							m_selectedEntity.RemoveComponent<RigidBodyComponent>();
						}

						ImGui::EndPopup();
					}

					RigidBodyPanel(rbc->ptr.get());
				}
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ShowAddComponentButton();
			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("Add Component");
			}
		}

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
			Model* model = rc->model.get();
			Shader* shader = rc->shader.get();
			if(ImGui::CollapsingHeader("Renderable Component"))
			{
				const std::string& modelIdent = model ? model->GetIdent() : "None";
				const std::string& shaderIdent = shader ? shader->GetIdent() : "None";

				ImGui::Checkbox("IsActive", &(rc->active));

				ImGui::PushID("ModelCombo");
				if (ImGui::BeginCombo("Model", modelIdent.c_str()))
				{
					std::map<std::string, SharedPtr<Model>>::const_iterator it;
					for (auto it = AssetManager<Model>::Instance().begin(); it != AssetManager<Model>::Instance().end(); ++it)
					{
						bool isSelected = shaderIdent == it->first;
						if (ImGui::Selectable(it->first.c_str(), isSelected))
						{
							rc->model = it->second;
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();

				ImGui::PushID("ShaderCombo");
				if (ImGui::BeginCombo("Shader", shaderIdent.c_str()))
				{
					std::map<std::string, SharedPtr<Shader>>::const_iterator it;
					for (auto it = AssetManager<Shader>::Instance().begin(); it != AssetManager<Shader>::Instance().end(); ++it)
					{
						bool isSelected = shaderIdent == it->first;
						if (ImGui::Selectable(it->first.c_str(), isSelected))
						{
							rc->shader = it->second;
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();
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
			PerspectiveCamera* camera = &cc->camera;
			if(ImGui::CollapsingHeader("Camera Component"))
			{
				if (ImGui::Button("Set Active"))
				{
					m_scene->SetActiveCamera(camera);
				}

				CameraPanel(camera);
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

	void Editor::ShowPlayerControllerComponent()
	{
		PlayerControllerComponent* pcc = m_selectedEntity.GetComponent<PlayerControllerComponent>();
		if(pcc != nullptr)
		{
			if(ImGui::CollapsingHeader("Player Controller Component"))
			{
				Properties props = pcc->ptr->GetControllerProperties();

				if(ImGui::DragFloat("Radius: ", &(props.radius), 0.1f, 0.0f, FLT_MAX, "%.3f"))
				{
					pcc->ptr->SetRadius(props.radius);
				}
				if(ImGui::DragFloat("Height: ", &(props.height), 0.1f, 0.0f, FLT_MAX, "%.3f"))
				{
					pcc->ptr->SetHeight(props.height);
				}
				if(ImGui::DragFloat("Speed: ", &(props.moveFactor), 0.1f, 0.0f, FLT_MAX, "%.3f"))
				{
					pcc->ptr->SetSpeed(props.moveFactor);
				}
				if(ImGui::DragFloat("Move Drag: ", &(props.moveDrag), 0.1f, 0.0f, FLT_MAX, "%.3f"))
				{
					pcc->ptr->SetDrag(props.moveDrag);
				}
				if(ImGui::DragFloat("Fall Drag: ", &(props.fallDrag), 0.1f, 0.0f, FLT_MAX, "%.3f"))
				{
					pcc->ptr->SetFallDrag(props.fallDrag);
				}
				if(ImGui::DragFloat3("Capsule Offset: ", &(props.capsuleOffset.x), 0.1f, 0.0f, 0.0f, "%.3f"))
				{
					pcc->ptr->SetCapsuleOffset(props.capsuleOffset);
				}
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
				ImGui::DragFloat3("Translation", &(translation->x), 0.001f, 0.0f, 0.0f, "%.4f");

				Math::quat* orientation = &rtc->orientation;
        		Math::vec3 eulerAnglesDegrees = Math::degrees(Math::eulerAngles(*orientation));
        		ImGui::DragFloat3("Rotation", &eulerAnglesDegrees.x, 0.001f, 0.0f, 0.0f, "%.4f");
        		*orientation = Math::quat(Math::radians(eulerAnglesDegrees));

				Math::vec3* scale = &rtc->scale;
				ImGui::DragFloat3("Scale", &(scale->x), 0.1f, 0.0f, FLT_MAX, "%.3f");

				Math::vec2* size = &rtc->size;
				ImGui::DragFloat2("Size", &(size->x), 1.0f, 0.0f, FLT_MAX, "%.3f");
			}
		}
	}

	void Editor::ShowCanvasRendererComponent()
	{
		CanvasRendererComponent* crc = m_selectedEntity.GetComponent<CanvasRendererComponent>();
		if(crc != nullptr)
		{
			if(ImGui::CollapsingHeader("Canvas Renderer Component"))
			{
				bool* active = &crc->active;
				ImGui::Checkbox("IsActive", active);

				bool* screenspace = &crc->screenSpace;
				ImGui::Checkbox("Screen Space", screenspace);

				bool* billboard = &crc->billboard;
				ImGui::Checkbox("Billboard", billboard);
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

	void Editor::ShowBDIComponent()
	{
		BDIComponent* bdi = m_selectedEntity.GetComponent<BDIComponent>();
		if(bdi != nullptr)
		{
			BDIAgent* agent = bdi->ptr.get();
			if (!agent)
			{
				bdi->ptr = MakeShared<BDIAgent>(m_selectedEntity.GetComponent<TagComponent>()->tag);
				agent = bdi->ptr.get();
			}

			if(ImGui::CollapsingHeader("BDI Component"))
			{
				ImGui::SeparatorText("General Properties");
				ImGui::Text("Name: %s", agent->GetName().c_str());
				float activationLevel = agent->GetActivationLevel();
				float intentionThreshold = agent->GetIntentionThreshold();
				if (ImGui::SliderFloat("Activation Level", &activationLevel, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
				{
					agent->SetActivationLevel(activationLevel);
				}
				ImGui::SameLine(); HelpMarker("[0, 1] showing the current activation level of the agent");
				if (ImGui::SliderFloat("Intention Threshold", &intentionThreshold, 0.0f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
				{
					agent->SetIntentionThreshold(intentionThreshold);
				}
				ImGui::SameLine(); HelpMarker("[0, 10] override threshold for intention selection");

				ImGui::Spacing();
				ImGui::Spacing();
				if (ImGui::TreeNode("Show Active"))
				{
					ImGui::SeparatorText("Beliefs");
					{
						// to add a belief
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							ImGui::OpenPopup("Add Belief##BDI");
						}

						if (ImGui::BeginPopup("Add Belief##BDI"))
						{
							static char name[32] = "Entity";
							ImGui::InputText("Belief", name, IM_ARRAYSIZE(name));

							if (ImGui::Button("Add", ImVec2(120, 0)))
							{
								agent->AddBelief(name);
							}

							ImGui::SameLine();

							if (ImGui::Button("Cancel", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
							}

							ImGui::EndPopup();
						}

						ImGui::Columns(2);
						const std::set<std::string>& beliefs = agent->GetBeliefs();
						for (auto it = beliefs.begin(); it != beliefs.end(); ++it)
						{
							ImGui::NextColumn();
						}

						ImGui::Columns(1);
						ImGui::Spacing();
						ImGui::Spacing();
					}

					ImGui::SeparatorText("Active Desires");
					{
						ImGui::Columns(2);
						const std::vector<BDIAgent::Concept>& desires = agent->GetActiveDesires();
						for (auto it = desires.begin(); it != desires.end(); ++it)
						{
							ImGui::Text("%s", it->first.c_str());
							ImGui::NextColumn();
							ImGui::Text("%f", it->second);
							ImGui::NextColumn();
						}

						ImGui::Columns(1);
						ImGui::Spacing();
						ImGui::Spacing();
					}

					ImGui::SeparatorText("Active Intentions");
					{
						ImGui::Columns(2);
						const std::vector<BDIAgent::Concept>& intentions = agent->GetActiveIntentions();
						for (auto it = intentions.begin(); it != intentions.end(); ++it)
						{
							ImGui::Text("%s", it->first.c_str());
							ImGui::NextColumn();
							ImGui::Text("%f", it->second);
							ImGui::NextColumn();
						}

						ImGui::Columns(1);
						ImGui::Spacing();
						ImGui::Spacing();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Show Potential"))
				{
					ImGui::SeparatorText("Potential Desires");
					{
						ImGui::Columns(2);
						const std::vector<BDIAgent::Concept>& desires = agent->GetPotentialDesires();
						for (auto it = desires.begin(); it != desires.end(); ++it)
						{
							ImGui::Text("%s", it->first.c_str());
							ImGui::NextColumn();
							ImGui::Text("%f", it->second);
							ImGui::NextColumn();
						}

						ImGui::Columns(1);
						ImGui::Spacing();
						ImGui::Spacing();
					}

					ImGui::SeparatorText("Potential Intentions");
					{
						ImGui::Columns(2);
						std::vector<std::string> intentions = agent->GetPotentialIntentions();
						for (auto it = intentions.begin(); it != intentions.end(); ++it)
						{
							ImGui::Text("%s", it->c_str());
							ImGui::NextColumn();
						}

						ImGui::Columns(1);
						ImGui::Spacing();
						ImGui::Spacing();
					}

					ImGui::TreePop();
				}

				ImGui::Spacing();
				ImGui::Spacing();
			}
		}
	}

	void Editor::ShowNavigationComponent()
	{
		static bool fetched = false;
		static float tileSize = 0.0f;
		static Math::ivec2 gridSize;
		static Math::vec3 position;
		static char filename[256] = "";

		NavigationGridComponent* tc = m_selectedEntity.GetComponent<NavigationGridComponent>();
		if(tc != nullptr)
		{
			if(ImGui::CollapsingHeader("Navigation Grid Component"))
			{
				if(!fetched)
				{
					tileSize = tc->grid->GetTileSize();
					gridSize = tc->grid->GetGridSize();
					position = tc->grid->GetPosition();
					fetched = true;
				}

				if (ImGui::BeginPopup("Grid Selection"))
				{
					std::map<std::string, SharedPtr<Grid>>::const_iterator it;
					for (it = AssetManager<Grid>::Instance().begin(); it != AssetManager<Grid>::Instance().end(); ++it)
					{
						if (ImGui::MenuItem(it->first.c_str()))
						{
							tc->grid = it->second;
						}
					}
					ImGui::EndPopup();
				}

				ImGui::DragFloat("Tile Size", &tileSize, 0.1f, 0.0f, FLT_MAX, "%.3f");
				ImGui::DragInt2("Grid Size", &gridSize.x, 1.0f, 0, INT_MAX, "%d");
				ImGui::DragFloat3("Position", &position.x, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");

				if (ImGui::Button("Resize Grid"))
				{
					tc->grid->ResizeGrid(gridSize, tileSize, position);
					fetched = false;
				}

				Grid* grid = tc->grid.get();
				const std::string& gridIdent = grid ? grid->GetIdent() : "None";

				if (ImGui::Button(gridIdent.c_str()))
				{
					ImGui::OpenPopup("Grid Selection");
				}

    			ImGui::InputText("Filename", filename, IM_ARRAYSIZE(filename));

				if (ImGui::Button("Save Grid"))
					tc->grid->SaveToFile("assets/grid/" + std::string(filename) + ".grid");

				ImGui::Separator();

				if (ImGui::CollapsingHeader("Grid Editor"))
				{
					Math::ivec2 size = tc->grid->GetGridSize();

					for (int i = 0; i < size.y; ++i)
					{
						ImGui::Columns(size.x, nullptr, false);

						for (int j = 0; j < size.x; ++j)
						{
							bool isWalkable = tc->grid->IsActive(j, i);

							std::string text = isWalkable ? "W" : "-";
							std::string id = "##" + std::to_string(i) + std::to_string(j);

							if (ImGui::Selectable((text + id).c_str(), false, ImGuiSelectableFlags_None))
								tc->grid->SetActive(j, i);

							ImGui::NextColumn();
						}
						ImGui::Separator();
						ImGui::Columns(1);
					}

					if (ImGui::Button("Update Grid"))
					{
						tc->grid->GenerateGrid();
						fetched = false;
					}
				}
			}
		}
	}

	void Editor::ShowFCMComponent()
	{
		FCMComponent* fcmComp = m_selectedEntity.GetComponent<FCMComponent>();
		if(fcmComp != nullptr)
		{
			if(ImGui::CollapsingHeader("FCM Component"))
			{				
				// get the fcm
				FCM* fcm = fcmComp->ptr.get();
				if (!fcm)
				{
					fcmComp->ptr = MakeShared<FCM>();
					fcm = fcmComp->ptr.get();
				}

				unsigned int numConcepts = fcm->GetConceptCount();
				if (numConcepts == 0)
				{
					ImGui::Text("No concepts");
					return;
				}

				// get the concepts for the fcm
				ImGui::BeginTable("Concepts", 4);
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Level");
				ImGui::TableSetupColumn("Threshold");
				ImGui::TableSetupColumn("State");
				ImGui::TableHeadersRow();

				const std::vector<Concept>& concepts = fcm->GetConcepts();
				for (const Concept &concept : concepts)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("%s", concept.name.c_str());
					ImGui::TableNextColumn();
					float activation = fcm->GetConceptValue(concept.name);
					ImGui::Text("%.3f", activation);
					ImGui::TableNextColumn();
					ImGui::Text("%.3f", concept.activationThreshold);
					ImGui::TableNextColumn();
					ImGui::Text("%s", concept.active ? "active" : "inactive");
				}
				ImGui::EndTable();
				ImGui::Spacing();
				ImGui::Spacing();

				// get the weights for the fcm
				ImGui::SeparatorText("Weights");
				const std::vector<float>& weights = fcm->GetWeights();

				ImGui::Columns(numConcepts);
				for (unsigned int i = 0; i < numConcepts; ++i)
				{
					for (unsigned int j = 0; j < numConcepts; ++j)
					{
						ImGui::Text("%f", weights[i * numConcepts + j]);
						ImGui::NextColumn();
					}
				}
				ImGui::Columns(1);
				ImGui::Spacing();
				ImGui::Spacing();
			}
		}
	}

	//--------------------------------------------------------------------------------------------------
	// Physics
	//--------------------------------------------------------------------------------------------------
	void Editor::PhysicsPanel()
	{
		// show the physics debug
		PhysicsWorld* physicsWorld = m_scene->GetPhysicsWorld();
		const PhysicsRenderer* physicsRenderer = physicsWorld->GetRenderer();
		bool isShowingPhysicsDebug = physicsWorld->IsRenderingEnabled();
		ImGui::Checkbox("Enable Debug Rendering", &isShowingPhysicsDebug);
		physicsWorld->SetRenderingEnabled(isShowingPhysicsDebug);

		static bool showAdditional;
		if(ImGui::Checkbox("Additional Debug Information", &showAdditional))
		{
			SceneManager::GetActiveScene()->SetAdditionalDebugging(showAdditional);
		}

		if (ImGui::TreeNode("Render Items"))
		{
			// Get the current state of the render items
			bool colliderAABB = physicsRenderer->IsRenderItemEnabled(PhysicsRendererItem::ColliderAABB);
			bool colliderBroadphaseAABB = physicsRenderer->IsRenderItemEnabled(PhysicsRendererItem::ColliderBroadphaseAABB);
			bool collisionShape = physicsRenderer->IsRenderItemEnabled(PhysicsRendererItem::CollisionShape);
			bool contactPoint = physicsRenderer->IsRenderItemEnabled(PhysicsRendererItem::ContactPoint);
			bool contactNormal = physicsRenderer->IsRenderItemEnabled(PhysicsRendererItem::ContactNormal);

			// provide a checkbox for each render item
			if (ImGui::Checkbox("Collider AABB", &colliderAABB))
			{
				physicsRenderer->SetRenderItem(PhysicsRendererItem::ColliderAABB, colliderAABB);
			}
			if (ImGui::Checkbox("Collider Broadphase AABB", &colliderBroadphaseAABB))
			{
				physicsRenderer->SetRenderItem(PhysicsRendererItem::ColliderBroadphaseAABB, colliderBroadphaseAABB);
			}
			if (ImGui::Checkbox("Collision Shape", &collisionShape))
			{
				physicsRenderer->SetRenderItem(PhysicsRendererItem::CollisionShape, collisionShape);
			}
			if (ImGui::Checkbox("Contact Point", &contactPoint))
			{
				physicsRenderer->SetRenderItem(PhysicsRendererItem::ContactPoint, contactPoint);
			}
			if (ImGui::Checkbox("Contact Normal", &contactNormal))
			{
				physicsRenderer->SetRenderItem(PhysicsRendererItem::ContactNormal, contactNormal);
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Render Shapes"))
		{
			// Get the current state of the render shapes
			bool box = physicsRenderer->IsRenderShapeEnabled(CollisionRenderShape::Box);
			bool sphere = physicsRenderer->IsRenderShapeEnabled(CollisionRenderShape::Sphere);
			bool capsule = physicsRenderer->IsRenderShapeEnabled(CollisionRenderShape::Capsule);

			// provide a checkbox for each render shape
			if (ImGui::Checkbox("Box", &box))
			{
				physicsRenderer->SetRenderShape(CollisionRenderShape::Box, box);
			}
			if (ImGui::Checkbox("Sphere", &sphere))
			{
				physicsRenderer->SetRenderShape(CollisionRenderShape::Sphere, sphere);
			}
			if (ImGui::Checkbox("Capsule", &capsule))
			{
				physicsRenderer->SetRenderShape(CollisionRenderShape::Capsule, capsule);
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}
	}

	void Editor::ColliderPanel(CollisionBody* body, Collider* collider, const char* label)
	{
		if (!collider)
		{
			return;
		}

		if (ImGui::TreeNode(label))
		{
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Remove Collider"))
				{
					body->RemoveCollider(collider);
					ImGui::EndPopup();
					ImGui::TreePop();
					return;
				}

				ImGui::EndPopup();
			}

			ImGui::Spacing();
			ImGui::Spacing();

			/// \todo Implement collider trigger functionality, currently causing a assertion
			// bool isTrigger = collider->GetIsTrigger();
			// ImGui::Checkbox("Is Trigger", &isTrigger);
			// collider->SetIsTrigger(isTrigger);

			// General configurations
			Math::vec3 offset = collider->GetOffset();
			Math::vec3 orientation = Math::degrees(Math::eulerAngles(collider->GetOrientation()));
			if (ImGui::DragFloat3("Offset", &offset.x, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				collider->SetOffset(offset);
			}
			if (ImGui::DragFloat3("Orientation", &orientation.x, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				collider->SetOrientation(Math::quat(Math::radians(orientation)));
			}

			// Specific configurations
			switch (collider->GetType())
			{
			case Collider::Type::Box:
				{
					// Box collider configurations
					Math::vec3 size = dynamic_cast<BoxCollider*>(collider)->GetSize();
					if (ImGui::DragFloat3("Size", &size.x, 0.1f, 0.0f, FLT_MAX, "%.3f"))
					{
						dynamic_cast<BoxCollider*>(collider)->Resize(size);
					}
				}
				break;
			case Collider::Type::Sphere:
				{
					// Sphere collider configurations
					float radius = dynamic_cast<SphereCollider*>(collider)->GetRadius();
					if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, FLT_MAX, "%.3f"))
					{
						dynamic_cast<SphereCollider*>(collider)->SetRadius(radius);
					}
				}
				break;
			case Collider::Type::Capsule:
				{
					// Capsule collider configurations
					float height = dynamic_cast<CapsuleCollider*>(collider)->GetHeight();
					float radius = dynamic_cast<CapsuleCollider*>(collider)->GetRadius();
					if (ImGui::DragFloat("Height", &height, 0.1f, 0.0f, FLT_MAX, "%.3f"))
					{
						dynamic_cast<CapsuleCollider*>(collider)->SetHeight(height);
					}
					if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, FLT_MAX, "%.3f"))
					{
						dynamic_cast<CapsuleCollider*>(collider)->SetRadius(radius);
					}
				}
				break;
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}
	}

	void Editor::CollisionBodyPanel(CollisionBody* body)
	{
		if (!body)
		{
			return;
		}

		// show transform
		if (ImGui::TreeNode("Transform"))
		{
			ImGui::Spacing();
			ImGui::Spacing();

			Math::vec3 position;
			Math::quat orientation;
			body->GetTransform(position, orientation);
			Math::vec3 eulerAnglesDegrees = Math::degrees(Math::eulerAngles(orientation));
			bool posEdit = ImGui::DragFloat3("Position", &position.x, 0.1f, 0.0f, 0.0f, "%.3f");
			bool rotEdit = ImGui::DragFloat3("Orientation", &eulerAnglesDegrees.x, 0.1f, 0.0f, 0.0f, "%.3f");
			if (posEdit || rotEdit)
			{
				body->SetTransform(position, Math::quat(Math::radians(eulerAnglesDegrees)));
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}

		ImGui::SeparatorText("Colliders");
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("Collider Context Menu##CollisionBody");
		}

		// if (ImGui::TreeNode("Colliders"))
		// {
			ImGui::Spacing();
			ImGui::Spacing();


			// Add collider popup
			if (ImGui::BeginPopup("Collider Context Menu##CollisionBody"))
			{
				Math::vec3 position;
				Math::quat orientation;
				body->GetTransform(position, orientation);

				if (ImGui::MenuItem("Add Box Collider"))
				{
					body->AddBoxCollider(Math::vec3(1.0f, 1.0f, 1.0f), Math::vec3(0.0f, 0.0f, 0.0f), orientation);
					// ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Add Sphere Collider"))
				{
					body->AddSphereCollider(1.0f, Math::vec3(0.0f, 0.0f, 0.0f), orientation);
					// ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Add Capsule Collider"))
				{
					body->AddCapsuleCollider(1.0f, 1.0f, Math::vec3(0.0f, 0.0f, 0.0f), orientation);
					// ImGui::CloseCurrentPopup();
				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				if (ImGui::MenuItem("Remove All"))
				{
					auto colliders = body->GetColliders();
					for (auto collider : colliders)
					{
						body->RemoveCollider(collider.get());
					}
				}
				ImGui::PopStyleColor();

				ImGui::EndPopup();
			}

			// Show all colliders
			auto colliders = body->GetColliders();
			Size_t i = 0;
			for (auto collider : colliders)
			{
				ColliderPanel(body, collider.get(), std::string(std::to_string(i) + " - " + collider->GetName()).c_str());
				++i;
			}


			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			// ImGui::TreePop();
		// }
	}

	void Editor::RigidBodyPanel(RigidBody* body)
	{
		if (!body)
		{
			return;
		}

		// show common panel first
		CollisionBodyPanel(dynamic_cast<CollisionBody*>(body));

		if (ImGui::TreeNode("Physical Properties"))
		{
			ImGui::Spacing();
			ImGui::Spacing();

			float mass = body->GetMass();
			float restitution = body->GetRestitution();
			Math::mat3 inertiaTensor = body->GetLocalInertiaTensor();
			Math::vec3 inertiaTensorDiagonal = Math::vec3(inertiaTensor[0][0], inertiaTensor[1][1], inertiaTensor[2][2]);
			Math::vec3 centerOfMass = body->GetCentreOfMass();

			if (ImGui::DragFloat("Mass (kg)", &mass, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				body->SetMass(mass);
			}
			if (ImGui::SliderFloat("Restitution", &restitution, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			{
				body->SetRestitution(restitution);
			}
			ImGui::InputFloat3("Inertia Tensor", &inertiaTensorDiagonal[0], "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Center of Mass", &centerOfMass[0], "%.3f", ImGuiInputTextFlags_ReadOnly);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Simulation Properties"))
		{
			ImGui::Spacing();
			ImGui::Spacing();

			int rbType = static_cast<int>(body->GetType());
			bool hasGravity = body->GetHasGravity();
			float linearDamping = body->GetLinearDamping();
			float angularDamping = body->GetAngularDamping();

			ImGui::RadioButton("Dynamic", &rbType, static_cast<int>(RigidBody::Type::Dynamic));
			ImGui::SameLine();
			ImGui::RadioButton("Static", &rbType, static_cast<int>(RigidBody::Type::Static));
			ImGui::SameLine();
			ImGui::RadioButton("Kinematic", &rbType, static_cast<int>(RigidBody::Type::Kinematic));
			body->SetType(static_cast<RigidBody::Type>(rbType));

			ImGui::SameLine();
			if (ImGui::Checkbox("Has Gravity", &hasGravity))
			{
				body->SetHasGravity(hasGravity);
			}

			if (ImGui::SliderFloat("Linear Damping", &linearDamping, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			{
				body->SetLinearDamping(linearDamping);
			}
			if (ImGui::SliderFloat("Angular Damping", &angularDamping, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			{
				body->SetAngularDamping(angularDamping);
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Velocity"))
		{
			ImGui::Spacing();
			ImGui::Spacing();

			Math::vec3 linearVelocity = body->GetLinearVelocity();
			Math::vec3 angularVelocity = body->GetAngularVelocity();
			Math::vec3 dps = Math::degrees(angularVelocity);
			Math::vec3 rpm = dps / 6.0f;

			if (ImGui::DragFloat3("Linear Velocity", &linearVelocity[0], 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				body->SetLinearVelocity(linearVelocity);
			}
			ImGui::SameLine();
			ImGui::Text("(%.3f m/s)", Math::length(linearVelocity));
			if (ImGui::DragFloat3("Angular Velocity", &angularVelocity[0], 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				body->SetAngularVelocity(angularVelocity);
			}
			ImGui::SameLine();
			ImGui::Text("(%.3f rad/s)", Math::length(angularVelocity));

			ImGui::InputFloat3("Degrees/Second", &dps[0], "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Revolutions/Minute", &rpm[0], "%.3f", ImGuiInputTextFlags_ReadOnly);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Momentum"))
		{
			ImGui::Spacing();
			ImGui::Spacing();

			Math::vec3 linearMomentum = body->GetLinearMomentum();
			Math::vec3 angularMomentum = body->GetAngularMomentum();
			if (ImGui::DragFloat3("Linear Momentum", &linearMomentum[0], 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				body->SetLinearMomentum(linearMomentum);
			}
			ImGui::SameLine();
			ImGui::Text("(%.3f kg m/s)", Math::length(linearMomentum));
			if (ImGui::DragFloat3("Angular Momentum", &angularMomentum[0], 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				body->SetAngularMomentum(angularMomentum);
			}
			ImGui::SameLine();
			ImGui::Text("(%.3f kg m^2/s)", Math::length(angularMomentum));

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
	}

	void Editor::HelpMarker(const char *label)
	{
		// from ImGui::Demo
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(label);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}
