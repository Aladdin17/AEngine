/**
 * \file Editor.h
 * \author Geoff Candy (34183006)
*/
#pragma once
#include <AEngine/Core/Window.h>
#include <AEngine/Scene/Entity.h>

namespace AEngine
{
	class Scene;

	/**
	 * \struct EditorProperties
	 * @brief properties for the editor
	 */
	struct EditorProperties
	{
		bool TitleBarMove = true;
		bool IsDockingEnabled = true;
		bool IsViewportEnabled = true;
	};

	/**
	 * @class Editor
	 * @brief Class to create the editor overlay
	 */
	class Editor
	{
	public:
			/**
			* @brief Initialisation of ImGui for the Editor
			* @param window
			* @param props
			*/
		void Init(Window* window, const EditorProperties& props);
			/**
			* @brief Create a New ImGui Frame
			*/
		void CreateNewFrame();
			/**
			* @brief Update for the Editor
			*/
		void Update();
			/**
			* @brief Render call for the ImGui elements in the Editor
			*/
		void Render();
			/**
			* @brief Shutdown Method for ImGui
			*/
		void Shutdown();

	private:
		Scene* m_scene;
		Entity m_selectedEntity;

		void ShowMat3(const char *label, Math::mat3& matrix);

			/**
			* @brief Method to create an ImGui Frame for the Debug Camera
			*/
		void ShowDebugCameraConfig();
			/**
			* @brief Method to create an ImGui Frame for Scene Debug Information
			*/
		void ShowDebugWindow();
			/**
			* @brief Method to create an ImGui Frame for the Entity Heirarchy
			*/
		void ShowHierarchy();
			/**
			* @brief Method to create an ImGui Frame for the Component Inspector
			*/
		void ShowInspector();

//------------------------------------------------------------------------------
// Methods for the Components in ECS
//------------------------------------------------------------------------------
			/**
			* @brief Method to create an ImGui Frame for the Tag Component
			*/
		void ShowTagComponent();
			/**
			* @brief Method to create an ImGui Frame for the Transform Component
			*/
		void ShowTransformComponent();
			/**
			* @brief Method to create an ImGui Frame for the Renderable Component
			*/
		void ShowRenderableComponent();
			/**
		 	* @brief Method to create an ImGui Frame for the Skinned Renderable Component
		 	*/
		void ShowSkinnedRenderableComponent();
			/**
			 * @brief Method to create an ImGui Frame for the Sky Box Component
			 */
		void ShowSkyboxComponent();
			/**
			 * @brief Method to create an ImGui Frame for the Camera Component
			 */
		void ShowCameraComponent();
			/**
			 * @brief Method to create an ImGui Frame for the Scriptable Component
			 */
		void ShowScriptableComponent();
			/**
			 * @brief Method to create an ImGui Frame for the Rigid Body Component
			 */
		void ShowRigidBodyComponent();
			/**
			 * @brief Method to create an ImGui Frame for the Collision Body Component
			 */
		void ShowCollisionBodyComponent();
			/**
			 * @brief Method to create an ImGui Frame for the Player Controller Component
			 */
		void ShowPlayerControllerComponent();

		void ShowRectTransformComponent();
		void ShowCanvasRendererComponent();
		void ShowPanelComponent();
		void ShowTextComponent();

		void ShowAddComponentButton();

		template <typename T>
		void ShowAddComponentPrompt(const char* label);

		//other editor features play and pause?
	};
}
