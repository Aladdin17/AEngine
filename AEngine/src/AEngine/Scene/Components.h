/**
 * @file
 * @brief Component supported by the ECS
 * @author Geoff Candy (34183006)
 * @author Christien Alden (34119981)
 **/
#pragma once
#include "AEngine/Core/PerspectiveCamera.h"
#include "AEngine/Core/Types.h"
#include "AEngine/Math/Math.h"
#include "AEngine/Physics/Collider.h"
#include "AEngine/Physics/CollisionBody.h"
#include "AEngine/Render/HeightMap.h"
#include "AEngine/Render/Model.h"
#include "AEngine/Render/Shader.h"
#include "AEngine/Render/Font.h"
#include "AEngine/Script/EntityScript.h"
#include "AEngine/AI/Grid.h"
#include <string>

namespace AEngine
{
	class Shader;
	class Texture;
	class RigidBody;
	class PlayerController;
	class Skybox;
	class Water;

	struct TagComponent
	{
		std::string tag;
		Uint16 ident;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag, Uint16 ident)
			: tag(tag), ident(ident) {}
	};

	struct TransformComponent
	{
		Math::vec3 translation{ 0.0f, 0.0f, 0.0f };
		Math::quat orientation{ Math::vec3(0.0f, 0.0f, 0.0f) };
		Math::vec3 scale{ 1.0f, 1.0f, 1.0f };

		const Math::mat4 ToMat4() const
		{
			return (
				Math::translate(Math::mat4(1.0f), translation)
				* Math::toMat4(orientation)
				* Math::scale(Math::mat4(1.0f), scale)
			);
		}

		Math::vec3 GetLocalX() const
		{
			return Math::normalize(Math::rotateVec(Math::vec3(1.0f, 0.0f, 0.0f), orientation));
		}

		Math::vec3 GetLocalY() const
		{
			return Math::normalize(Math::rotateVec(Math::vec3(0.0f, 1.0f, 0.0f), orientation));
		}

		Math::vec3 GetLocalZ() const
		{
			return Math::normalize(Math::rotateVec(Math::vec3(0.0f, 0.0f, 1.0f) , orientation));
		}

		void LookAt(const Math::vec3& target)
		{
			Math::vec3 direction = Math::normalize(target - translation);
			Math::vec3 localZ = -direction;
			Math::vec3 localX = Math::normalize(Math::cross(Math::vec3(0.0f, 1.0f, 0.0f), localZ));
			Math::vec3 localY = Math::normalize(Math::cross(localZ, localX));
			Math::mat3 rotationMatrix{localX, localY, localZ};
			orientation = Math::quat_cast(rotationMatrix);
		}
	};

	struct NavigationGridComponent
	{
		bool debug;
		SharedPtr<Grid> grid = nullptr;
	};

	struct CanvasRendererComponent
	{
		bool active;
		bool screenSpace; // else world space
		bool billboard; // Always face the camera
	};

	struct RectTransformComponent : TransformComponent
	{
		Math::vec2 size;
	};

	struct TextComponent
	{
		SharedPtr<Font> font;
		std::string text;
		Math::vec4 color;
	};

	struct PanelComponent
	{
		SharedPtr<Texture> texture;
		Math::vec4 color;
	};

	struct RenderableComponent
	{
		bool active;
		SharedPtr<Model> model;
		SharedPtr<Shader> shader;
	};

	struct SkinnedRenderableComponent
	{
		bool active;
		SharedPtr<Model> model;
		SharedPtr<Shader> shader;
		Animator animator;
	};

	struct SkyboxComponent
	{
		bool active;
		SharedPtr<Skybox> skybox;
		SharedPtr<Shader> shader;
	};

	struct CameraComponent
	{
		PerspectiveCamera camera;
	};

	struct ScriptableComponent
	{
		UniquePtr<EntityScript> script;
	};

	struct CollisionBodyComponent
	{
		SharedPtr<CollisionBody> ptr;
	};

	struct RigidBodyComponent
	{
		SharedPtr<RigidBody> ptr;
	};

	struct BoxColliderComponent
	{
		// runtime
		Collider *ptr = nullptr;

		// config
		bool isTrigger;
		Math::vec3 size;
	};

	struct PlayerControllerComponent
	{
		float radius;
		float height;
		float speed;
		float moveDrag;
		float fallDrag;

		PlayerController* ptr;
	};	
}
