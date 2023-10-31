#include <string>
#include <vector>

#include "ScriptEngineImpl.h"
#include "AEngine/Core/Application.h"
#include "AEngine/Core/PerspectiveCamera.h"
#include "AEngine/Core/Timer.h"
#include "AEngine/Core/TimeStep.h"
#include "AEngine/Core/Types.h"
#include "AEngine/FSM/FSM.h"
#include "AEngine/FSM/FSMState.h"
#include "AEngine/Input/KeyCodes.h"
#include "AEngine/Math/Math.h"
#include "AEngine/Physics/PlayerController.h"
#include "AEngine/Resource/AssetManager.h"
#include "AEngine/Scene/Components.h"
#include "AEngine/Scene/DebugCamera.h"
#include "AEngine/Scene/Entity.h"
#include "AEngine/Scene/Scene.h"
#include "AEngine/Scene/SceneManager.h"
#include "AEngine/Messaging/MessageService.h"
#include "AEngine/Render/Animation.h"
#include "AEngine/AI/Grid.h"
#include "AEngine/AI/FCM.h"

namespace AEngine
{
	ScriptEngineImpl& ScriptEngineImpl::Instance()
	{
		static ScriptEngineImpl instance;
		return instance;
	}

	void ScriptEngineImpl::LoadFile(const std::string& path)
	{
		m_state.LoadFile(path);
	}

	void ScriptEngineImpl::LoadScript(const std::string &script)
	{
		m_state.LoadScript(script);
	}

	void ScriptEngineImpl::Shutdown()
	{
		/// \todo Implement ScriptEngineImpl::Shutdown
	}

	ScriptState &ScriptEngineImpl::GetState()
	{
		return m_state;
	}

//--------------------------------------------------------------------------------
// Input Module
//--------------------------------------------------------------------------------
	void RegisterInputPolling(sol::state& state)
	{
		state["GetKey"] = [](AEKey key) -> AEInputState {
			return Application::Instance().GetInput().GetKey(key);
		};

		state["IsKeyDown"] = [](AEKey key) -> bool {
			return Application::Instance().GetInput().IsKeyDown(key);
		};

		state["IsKeyUp"] = [](AEKey key) -> bool {
			return Application::Instance().GetInput().IsKeyUp(key);
		};

		state["GetMouseButton"] = [](AEMouse mouse) -> AEInputState {
			return Application::Instance().GetInput().GetMouseButton(mouse);
		};

		state["IsMouseButtonDown"] = [](AEMouse mouse) -> bool {
			return Application::Instance().GetInput().IsMouseButtonDown(mouse);
		};

		state["IsMouseButtonUp"] = [](AEMouse mouse) -> bool {
			return Application::Instance().GetInput().IsMouseButtonUp(mouse);
		};

		state["GetMouseDelta"] = []() -> Math::vec2 {
			return Application::Instance().GetInput().GetMouseDelta();
		};

		state["GetMousePosition"] = []() -> Math::vec2 {
			return Application::Instance().GetInput().GetMousePosition();
		};

		state["GetMouseScroll"] = []() -> Math::vec2 {
			return Application::Instance().GetInput().GetMouseScroll();
		};
	}

	void RegisterInputStates(sol::state& state)
	{
		state.new_enum<AEInputState>("AEInput", {
			{"Unknown", AEInputState::Unknown},
			{"Pressed", AEInputState::Pressed},
			{"Released", AEInputState::Released},
			{"Repeated", AEInputState::Repeated}
		});
	}

	void RegisterKeyCodes(sol::state& state)
	{
		state.new_enum<AEKey>("AEKey", {
			// alpha keys
			{"A", AEKey::A},
			{"B", AEKey::B},
			{"C", AEKey::C},
			{"D", AEKey::D},
			{"E", AEKey::E},
			{"F", AEKey::F},
			{"G", AEKey::G},
			{"H", AEKey::H},
			{"I", AEKey::I},
			{"J", AEKey::J},
			{"K", AEKey::K},
			{"L", AEKey::L},
			{"M", AEKey::M},
			{"N", AEKey::N},
			{"O", AEKey::O},
			{"P", AEKey::P},
			{"Q", AEKey::Q},
			{"R", AEKey::R},
			{"S", AEKey::S},
			{"T", AEKey::T},
			{"U", AEKey::U},
			{"V", AEKey::V},
			{"W", AEKey::W},
			{"X", AEKey::X},
			{"Y", AEKey::Y},
			{"Z", AEKey::Z},

			// number keys
			{"NUM0", AEKey::NUM0},
			{"NUM1", AEKey::NUM1},
			{"NUM2", AEKey::NUM2},
			{"NUM3", AEKey::NUM3},
			{"NUM4", AEKey::NUM4},
			{"NUM5", AEKey::NUM5},
			{"NUM6", AEKey::NUM6},
			{"NUM7", AEKey::NUM7},
			{"NUM8", AEKey::NUM8},
			{"NUM9", AEKey::NUM9},
			{"KP_0", AEKey::KP_0},
			{"KP_1", AEKey::KP_1},
			{"KP_2", AEKey::KP_2},
			{"KP_3", AEKey::KP_3},
			{"KP_4", AEKey::KP_4},
			{"KP_5", AEKey::KP_5},
			{"KP_6", AEKey::KP_6},
			{"KP_7", AEKey::KP_7},
			{"KP_8", AEKey::KP_8},
			{"KP_9", AEKey::KP_9},

			// special keys
			{"SPACE", AEKey::SPACE},
			{"APOSTROPHE", AEKey::APOSTROPHE},
			{"COMMA", AEKey::COMMA},
			{"MINUS", AEKey::MINUS},
			{"PERIOD", AEKey::PERIOD},
			{"SLASH", AEKey::SLASH},
			{"SEMICOLON", AEKey::SEMICOLON},
			{"EQUAL", AEKey::EQUAL},
			{"LEFT_BRACKET", AEKey::LEFT_BRACKET},
			{"BACKSLASH", AEKey::BACKSLASH},
			{"RIGHT_BRACKET", AEKey::RIGHT_BRACKET},
			{"GRAVE_ACCENT", AEKey::GRAVE_ACCENT},
			{"ESCAPE", AEKey::ESCAPE},
			{"ENTER", AEKey::ENTER},
			{"TAB", AEKey::TAB},
			{"BACKSPACE", AEKey::BACKSPACE},
			{"INSERT", AEKey::INSERT},
			{"DEL", AEKey::DEL},
			{"RIGHT", AEKey::RIGHT},
			{"LEFT", AEKey::LEFT},
			{"DOWN", AEKey::DOWN},
			{"UP", AEKey::UP},
			{"PAGE_UP", AEKey::PAGE_UP},
			{"PAGE_DOWN", AEKey::PAGE_DOWN},
			{"HOME", AEKey::HOME},
			{"END", AEKey::END},
			{"CAPS_LOCK", AEKey::CAPS_LOCK},
			{"SCROLL_LOCK", AEKey::SCROLL_LOCK},
			{"NUM_LOCK", AEKey::NUM_LOCK},
			{"PRINT_SCREEN", AEKey::PRINT_SCREEN},
			{"PAUSE", AEKey::PAUSE},
			{"F1", AEKey::F1},
			{"F2", AEKey::F2},
			{"F3", AEKey::F3},
			{"F4", AEKey::F4},
			{"F5", AEKey::F5},
			{"F6", AEKey::F6},
			{"F7", AEKey::F7},
			{"F8", AEKey::F8},
			{"F9", AEKey::F9},
			{"F10", AEKey::F10},
			{"F11", AEKey::F11},
			{"F12", AEKey::F12},
			{"F13", AEKey::F13},
			{"F14", AEKey::F14},
			{"F15", AEKey::F15},
			{"F16", AEKey::F16},
			{"F17", AEKey::F17},
			{"F18", AEKey::F18},
			{"F19", AEKey::F19},
			{"F20", AEKey::F20},
			{"F21", AEKey::F21},
			{"F22", AEKey::F22},
			{"F23", AEKey::F23},
			{"F24", AEKey::F24},
			{"F25", AEKey::F25},
			{"KP_DECIMAL", AEKey::KP_DECIMAL},
			{"KP_DIVIDE", AEKey::KP_DIVIDE},
			{"KP_MULTIPLY", AEKey::KP_MULTIPLY},
			{"KP_SUBTRACT", AEKey::KP_SUBTRACT},
			{"KP_ADD", AEKey::KP_ADD},
			{"KP_ENTER", AEKey::KP_ENTER},
			{"KP_EQUAL", AEKey::KP_EQUAL},
			{"LEFT_SHIFT", AEKey::LEFT_SHIFT},
			{"LEFT_CONTROL", AEKey::LEFT_CONTROL},
			{"LEFT_ALT", AEKey::LEFT_ALT},
			{"RIGHT_SHIFT", AEKey::RIGHT_SHIFT},
			{"RIGHT_CONTROL", AEKey::RIGHT_CONTROL},
			{"RIGHT_ALT", AEKey::RIGHT_ALT}
		});
	};

	void RegisterMouseCodes(sol::state& state)
	{
		state.new_enum<AEMouse>("AEMouse", {
			{"BUTTON1", AEMouse::BUTTON_1},
			{"BUTTON2", AEMouse::BUTTON_2},
			{"BUTTON3", AEMouse::BUTTON_3},
			{"BUTTON4", AEMouse::BUTTON_4},
			{"BUTTON5", AEMouse::BUTTON_5},
			{"BUTTON6", AEMouse::BUTTON_6},
			{"BUTTON7", AEMouse::BUTTON_7},
			{"BUTTON8", AEMouse::BUTTON_8},
			{"LEFT", AEMouse::BUTTON_LEFT},
			{"RIGHT", AEMouse::BUTTON_RIGHT},
			{"MIDDLE", AEMouse::BUTTON_MIDDLE}
		});
	}

	void RegisterInputModule(sol::state &state)
	{
		RegisterInputPolling(state);
		RegisterInputStates(state);
		RegisterKeyCodes(state);
		RegisterMouseCodes(state);
	}

//--------------------------------------------------------------------------------
// BasicTypes
//--------------------------------------------------------------------------------
	void RegisterStringVector(sol::state& state)
	{
		using string = std::string;
		using stringvec = std::vector<string>;

		auto pushback_overload = sol::overload(
			[](stringvec& vec, const char* str) {
				vec.push_back(str);
			},

			[](stringvec& vec, const string& str) {
				vec.push_back(str);
			}
		);

		auto at_overload = sol::overload(
			[](const stringvec& vec, int index) {
				return vec.at(index - 1);
			},

			[](stringvec& vec, int index) {
				return vec.at(index - 1);
			}
		);

		auto size = [](const stringvec& vec) -> int {
			return static_cast<int>(vec.size());
		};

		state.new_usertype<stringvec>(
			"StringVector",
			sol::constructors<
				stringvec(),
				stringvec(int)
			>(),
			"Clear", &stringvec::clear,
			"Size", &stringvec::size,
			"PushBack", pushback_overload,
			"At", at_overload,
			sol::meta_function::index, at_overload
		);
	}

	void RegisterVector3Array(sol::state& state)
	{
		using vector3array = std::vector<float>;

		auto pushback = [](vector3array& vec, const float str) {
			vec.push_back(str);
		};

		auto at_overload = sol::overload(
			[](const vector3array& vec, int index) {
				return vec.at(index - 1);
			},

			[](vector3array& vec, int index) {
				return vec.at(index - 1);
			}
			);

		state.new_usertype<vector3array>(
			"Vector3Array",
			sol::constructors<
			vector3array()
			>(),
			"Clear", &vector3array::clear,
			"Size", &vector3array::size,
			"PushBack", pushback,
			"At", at_overload,
			sol::meta_function::index, at_overload
		);
	}

	void RegisterTypesModule(sol::state& state)
	{
		RegisterStringVector(state);
		RegisterVector3Array(state);
	}

//--------------------------------------------------------------------------------
// Math Module
//--------------------------------------------------------------------------------
	void RegisterMathNamespace(sol::state& state)
	{
		auto rotate_overload = sol::overload(
			[](const Math::quat& q, float angle, const Math::vec3& axis) -> Math::quat {
				return Math::rotate(q, angle, axis);
			}
		);

		auto length_overload = sol::overload(
			[](const Math::vec2& v) -> float {
				return Math::length(v);
			},

			[](const Math::vec3& v) -> float {
				return Math::length(v);
			}
		);

		auto normalize_overload = sol::overload(
			[](const Math::vec2& v) -> Math::vec2 {
				return Math::normalize(v);
			},

			[](const Math::vec3& v) -> Math::vec3 {
				return Math::normalize(v);
			},

			[](const Math::quat& q) -> Math::quat {
				return Math::normalize(q);
			}
		);

		auto dotProduct_overload = sol::overload(
			[](const Math::vec2& v1, const Math::vec2& v2) -> float {
				return Math::dot(v1, v2);
			},

			[](const Math::vec3& v1, const Math::vec3& v2) -> float {
				return Math::dot(v1, v2);
			}
		);

		auto crossProduct_overload = sol::overload(
			[](const Math::vec3& v1, const Math::vec3& v2) -> Math::vec3 {
				return Math::cross(v1, v2);
			}
		);

		auto clamp = [](float value, float min, float max) -> float {
			return Math::clamp(value, min, max);
		};

		auto to_quat = [](Math::vec3 vector) -> Math::quat {
			return Math::quat(Math::radians(vector));
		};

		state["AEMath"] = state.create_table();
		state["AEMath"]["Rotate"] = rotate_overload;
		state["AEMath"]["RotateVec"] = Math::rotateVec;
		state["AEMath"]["Length"] = length_overload;
		state["AEMath"]["Normalize"] = normalize_overload;
		state["AEMath"]["Clamp"] = clamp;
		state["AEMath"]["Dot"] = dotProduct_overload;
		state["AEMath"]["Cross"] = crossProduct_overload;
		state["AEMath"]["Vec3ToQuat"] = to_quat;
	}

	void RegisterVec2(sol::state& state)
	{
		auto add_overload = sol::overload(
			[](const Math::vec2& v1, const Math::vec2& v2) -> Math::vec2 {
				return v1 + v2;
			},

			[](const Math::vec2& v, float f) -> Math::vec2 {
				return v + f;
			},

			[](float f, const Math::vec2& v) -> Math::vec2 {
				return f + v;
			}
		);

		auto sub_overload = sol::overload(
			[](const Math::vec2& v1, const Math::vec2& v2) -> Math::vec2 {
				return v1 - v2;
			},

			[](const Math::vec2& v, float f) -> Math::vec2 {
				return v - f;
			},

			[](float f, const Math::vec2& v) -> Math::vec2 {
				return f - v;
			}
		);

		auto mult_overload = sol::overload(
			[](const Math::vec2& v1, const Math::vec2& v2) -> Math::vec2 {
				return v1 * v2;
			},

			[](const Math::vec2& v, float f) -> Math::vec2 {
				return v * f;
			},

			[](float f, const Math::vec2& v) -> Math::vec2 {
				return f * v;
			}
		);

		auto div_overload = sol::overload(
			[](const Math::vec2& v1, const Math::vec2& v2) -> Math::vec2 {
				return v1 / v2;
			},

			[](const Math::vec2& v, float f) -> Math::vec2 {
				return v / f;
			},

			[](float f, const Math::vec2& v) -> Math::vec2 {
				return f / v;
			}
		);

		auto unary_minus = [](const Math::vec2& v) -> Math::vec2 {
			return -v;
		};

		auto equal_to = [](const Math::vec2& v1, const Math::vec2& v2) -> Math::vec2 {
			return Math::equal(v1, v2);
		};

		state.new_usertype<Math::vec2>(
			"Vec2",
			sol::constructors<
				Math::vec2(),
				Math::vec2(float, float),
				Math::vec2(const Math::vec2&)
			>(),
			"x", &Math::vec2::x,
			"y", &Math::vec2::y,
			sol::meta_function::addition, add_overload,
			sol::meta_function::subtraction, sub_overload,
			sol::meta_function::multiplication, mult_overload,
			sol::meta_function::division, div_overload,
			sol::meta_function::unary_minus, unary_minus,
			sol::meta_function::equal_to, equal_to
		);
	}

	void RegisterVec3(sol::state& state)
	{
		auto add_overload = sol::overload(
			[](const Math::vec3& v1, const Math::vec3& v2) -> Math::vec3 {
				return v1 + v2;
			},

			[](const Math::vec3& v, float f) -> Math::vec3 {
				return v + f;
			},

			[](float f, const Math::vec3& v) -> Math::vec3 {
				return f + v;
			}
		);

		auto sub_overload = sol::overload(
			[](const Math::vec3& v1, const Math::vec3& v2) -> Math::vec3 {
				return v1 - v2;
			},

			[](const Math::vec3& v, float f) -> Math::vec3 {
				return v - f;
			},

			[](float f, const Math::vec3& v) -> Math::vec3 {
				return f - v;
			}
		);

		auto mult_overload = sol::overload(
			[](const Math::vec3& v1, const Math::vec3& v2) -> Math::vec3 {
				return v1 * v2;
			},

			[](const Math::vec3& v, float f) -> Math::vec3 {
				return v * f;
			},

			[](float f, const Math::vec3& v) -> Math::vec3 {
				return f * v;
			}
		);

		auto div_overload = sol::overload(
			[](const Math::vec3& v1, const Math::vec3& v2) -> Math::vec3 {
				return v1 / v2;
			},

			[](const Math::vec3& v, float f) -> Math::vec3 {
				return v / f;
			},

			[](float f, const Math::vec3& v) -> Math::vec3 {
				return f / v;
			}
		);

		auto unary_minus = [](const Math::vec3& v) -> Math::vec3 {
			return -v;
		};

		auto equal_to = [](const Math::vec3& v1, const Math::vec3& v2) -> Math::vec3 {
			return Math::equal(v1, v2);
		};

		state.new_usertype<Math::vec3>(
			"Vec3",
			sol::constructors<
				Math::vec3(),
				Math::vec3(float, float, float),
				Math::vec3(const Math::vec3&)
			>(),
			"x", &Math::vec3::x,
			"y", &Math::vec3::y,
			"z", &Math::vec3::z,
			"Rotate", &Math::rotateVec,
			sol::meta_function::addition, add_overload,
			sol::meta_function::subtraction, sub_overload,
			sol::meta_function::multiplication, mult_overload,
			sol::meta_function::division, div_overload,
			sol::meta_function::unary_minus, unary_minus,
			sol::meta_function::equal_to, equal_to
		);
	}

	void RegisterVec4(sol::state& state)
	{
		auto add_overload = sol::overload(
			[](const Math::vec4& v1, const Math::vec4& v2) -> Math::vec4 {
				return v1 + v2;
			},

			[](const Math::vec4& v, float f) -> Math::vec4 {
				return v + f;
			},

			[](float f, const Math::vec4& v) -> Math::vec4 {
				return f + v;
			}
		);

		auto sub_overload = sol::overload(
			[](const Math::vec4& v1, const Math::vec4& v2) -> Math::vec4 {
				return v1 - v2;
			},

			[](const Math::vec4& v, float f) -> Math::vec4 {
				return v - f;
			},

			[](float f, const Math::vec4& v) -> Math::vec4 {
				return f - v;
			}
		);

		auto mult_overload = sol::overload(
			[](const Math::vec4& v1, const Math::vec4& v2) -> Math::vec4 {
				return v1 * v2;
			},

			[](const Math::vec4& v, float f) -> Math::vec4 {
				return v * f;
			},

			[](float f, const Math::vec4& v) -> Math::vec4 {
				return f * v;
			}
		);

		auto div_overload = sol::overload(
			[](const Math::vec4& v1, const Math::vec4& v2) -> Math::vec4 {
				return v1 / v2;
			},

			[](const Math::vec4& v, float f) -> Math::vec4 {
				return v / f;
			},

			[](float f, const Math::vec4& v) -> Math::vec4 {
				return f / v;
			}
		);

		auto unary_minus = [](const Math::vec4& v) -> Math::vec4 {
			return -v;
		};

		auto equal_to = [](const Math::vec4& v1, const Math::vec4& v2) -> Math::vec4 {
			return Math::equal(v1, v2);
		};

		state.new_usertype<Math::vec4>(
			"Vec4",
			sol::constructors<
				Math::vec4(),
				Math::vec4(float, float, float, float),
				Math::vec4(const Math::vec4&)
			>(),
			"x", &Math::vec4::x,
			"y", &Math::vec4::y,
			"z", &Math::vec4::z,
			"Rotate", &Math::rotateVec,
			sol::meta_function::addition, add_overload,
			sol::meta_function::subtraction, sub_overload,
			sol::meta_function::multiplication, mult_overload,
			sol::meta_function::division, div_overload,
			sol::meta_function::unary_minus, unary_minus,
			sol::meta_function::equal_to, equal_to
		);
	}

	void RegisterQuat(sol::state& state)
	{
		auto mult_overload = sol::overload(
			[](const Math::quat& q1, const Math::quat& q2) -> Math::quat {
				return q1 * q2;
			}
		);

		state.new_usertype<Math::quat>(
			"Quat",
			sol::constructors<
				Math::quat(),
				Math::quat(float, float, float, float)
			>(),
			"x", &Math::quat::x,
			"y", &Math::quat::y,
			"z", &Math::quat::z,
			"w", &Math::quat::w,
			sol::meta_function::multiplication, mult_overload
		);
	}

	void RegisterMathModule(sol::state &state)
	{
		RegisterMathNamespace(state);
		RegisterVec2(state);
		RegisterVec3(state);
		RegisterVec4(state);
		RegisterQuat(state);
	}

//--------------------------------------------------------------------------------
// Core Module
//--------------------------------------------------------------------------------
	void RegisterApplication(sol::state& state)
	{
		auto terminate = []() {
			Application::Instance().Terminate();
		};

		state.new_usertype<Application>(
			"Application",
			sol::no_constructor,
			"Terminate", terminate
		);
	}

	void RegisterPerspectiveCamera(sol::state &state)
	{
		state.new_usertype<PerspectiveCamera>(
			"PerspectiveCamera",
			sol::constructors<
				PerspectiveCamera(),
				PerspectiveCamera(float, float, float, float)
			>(),

			// setters
			"SetNearPlane", &PerspectiveCamera::SetNearPlane,
			"SetFarPlane", &PerspectiveCamera::SetFarPlane,
			"SetFov", &PerspectiveCamera::SetFov,
			"SetAspect", &PerspectiveCamera::SetAspect,

			// getters
			"GetNearPlane", &PerspectiveCamera::GetNearPlane,
			"GetFarPlane", &PerspectiveCamera::GetFarPlane,
			"GetFov", &PerspectiveCamera::GetFov,
			"GetAspect", &PerspectiveCamera::GetAspect
		);
	}

	void RegisterTimeStep(sol::state &state)
	{
		state.new_usertype<TimeStep>(
			"TimeStep",
			sol::constructors<
				TimeStep(),
				TimeStep(float)
			>(),
			"Seconds", &TimeStep::Seconds,
			"Milliseconds", &TimeStep::Milliseconds,
			"Microseconds", &TimeStep::Microseconds,
			"Nanoseconds", &TimeStep::Nanoseconds,
			sol::meta_function::addition, &TimeStep::operator+
		);
	}

	void RegisterTimer(sol::state &state)
	{
		state.new_usertype<Timer>(
			"Timer",
			sol::constructors<
				Timer()
			>(),
			"Start", &Timer::Start,
			"Stop", &Timer::Stop,
			"Reset", &Timer::Reset,
			"GetDelta", &Timer::GetDelta,
			"PeekDelta", &Timer::PeekDelta,
			"IsRunning", &Timer::IsRunning
		);
	}

	void RegisterCoreModule(sol::state &state)
	{
		RegisterApplication(state);
		RegisterPerspectiveCamera(state);
		RegisterTimeStep(state);
		RegisterTimer(state);
	}

//--------------------------------------------------------------------------------
// Scene Module
//--------------------------------------------------------------------------------
	void RegisterScene(sol::state& state)
	{
		auto getEntity_overload = sol::overload(
			[](Scene& scene, const std::string& name) -> Entity {
				return scene.GetEntity(name);
			},

			[](Scene& scene, Uint16 id) -> Entity {
				return scene.GetEntity(id);
			}
		);

		state.new_usertype<Scene>(
			"Scene",
			sol::no_constructor,

			// management
			"GetIdent", &Scene::GetIdent,
			"CreateEntity", &Scene::CreateEntity,
			"GetEntity", getEntity_overload,

			// events -> Maybe don't expose these??
			"OnUpdate", &Scene::OnUpdate,
			"OnViewportResize", &Scene::OnViewportResize,

			// simulation
			"SetActiveCamera", &Scene::SetActiveCamera,

			// debug camera
			"UseDebugCamera", &Scene::UseDebugCamera,
			"UsingDebugCamera", &Scene::UsingDebugCamera,
			"GetDebugCamera", &Scene::GetDebugCamera
		);
	}

	void RegisterDebugCamera(sol::state &state)
	{
		state.new_usertype<DebugCamera>(
			"DebugCamera",
			// constructors
			sol::constructors<
				DebugCamera(),
				DebugCamera(float, float, float, float)
			>(),

			// base classes
			sol::base_classes, sol::bases<PerspectiveCamera>(),

			// getters
			"GetPosition", &DebugCamera::GetPosition,
			"GetYaw", &DebugCamera::GetYaw,
			"GetPitch", &DebugCamera::GetPitch,
			"GetMovementSpeed", &DebugCamera::GetMovementSpeed,
			"GetLookSensitivity", &DebugCamera::GetLookSensitivity,

			// setters
			"SetPosition", &DebugCamera::SetPosition,
			"SetYaw", &DebugCamera::SetYaw,
			"SetPitch", &DebugCamera::SetPitch,
			"SetMovementSpeed", &DebugCamera::SetMovementSpeed,
			"SetLookSensitivity", &DebugCamera::SetLookSensitivity
		);
	}

	void RegisterSceneManager(sol::state& state)
	{
		state.new_usertype<SceneManager>(
			"SceneManager",
			"CreateScene", &SceneManager::CreateScene,
			"LoadFromFile", &SceneManager::LoadFromFile,
			"SaveActiveToFile", &SceneManager::SaveActiveToFile,
			"UnloadScene", &SceneManager::UnloadScene,
			"UnloadAllScenes", &SceneManager::UnloadAllScenes,
			"GetSceneIdents", &SceneManager::GetSceneIdents,
			"HasScene", &SceneManager::HasScene,
			"SetActiveScene", &SceneManager::SetActiveScene,
			"GetActiveScene", &SceneManager::GetActiveScene,
			"GetScene", &SceneManager::GetScene
		);
	}

	void RegisterSceneModule(sol::state &state)
	{
		RegisterScene(state);
		RegisterDebugCamera(state);
		RegisterSceneManager(state);
	}

//--------------------------------------------------------------------------------
// Entity Module
//--------------------------------------------------------------------------------
	void RegisterEntity(sol::state& state)
	{
		auto translateLocal = [](Entity* entity, const Math::vec3& translation) {
			if (Math::all(Math::equal(translation, Math::vec3(0.0f))))
			{
				return;
			}

			Math::quat& orient = (*entity->GetComponent<TransformComponent>()).orientation;
			Math::vec3& entityPos = (*entity->GetComponent<TransformComponent>()).translation;

			Math::vec3 worldTranslation = Math::rotateVec(translation, orient);
			entityPos += worldTranslation;
		};

		auto rotateLocal = [](Entity* entity, float angleRads, const Math::vec3& axis) {
			if (Math::all(Math::equal(axis, Math::vec3(0.0f))))
			{
				return;
			}

			Math::quat& orient = (*entity->GetComponent<TransformComponent>()).orientation;
			orient = Math::rotate(orient, angleRads, axis);
		};

		state.new_usertype<Entity>(
			"Entity",
			sol::constructors<Entity(entt::entity, Scene*)>(),
			"GetTransformComponent", &Entity::GetComponent<TransformComponent>,
			"GetRenderableComponent", &Entity::GetComponent<RenderableComponent>,
			"GetNavigationGridComponent", &Entity::GetComponent<NavigationGridComponent>,
			"GetCanvasRendererComponent", &Entity::GetComponent<CanvasRendererComponent>,
			"GetRectTransformComponent", &Entity::GetComponent<RectTransformComponent>,
			"GetPanelComponent", &Entity::GetComponent<PanelComponent>,
			"GetTagComponent", &Entity::GetComponent<TagComponent>,
			"AddTransformComponent", &Entity::AddComponent<TransformComponent>,
			"AddRenderableComponent", &Entity::AddComponent<RenderableComponent>,
			"GetPlayerControllerComponent", &Entity::GetComponent<PlayerControllerComponent>,
			"GetAnimationComponent", &Entity::GetComponent<SkinnedRenderableComponent>,
			"GetBDIComponent", &Entity::GetComponent<BDIComponent>,
			"GetFCMComponent", &Entity::GetComponent<FCMComponent>,
			"GetPhysicsBody", &Entity::GetComponent<RigidBodyComponent>,
			// "AddScriptableComponent", &Entity::AddComponent<ScriptableComponent>,
			"TranslateLocal", translateLocal,
			"RotateLocal", rotateLocal,
			"Destroy", &Entity::Destroy,
			"GetScene", &Entity::GetScene
		);
	}

	void RegisterCollisionBody(sol::state& state)
	{
		auto get_translation = [](RigidBodyComponent* body) -> Math::vec3 {
			Math::vec3 position;
			Math::quat orientation;
			body->ptr->GetTransform(position, orientation);
			return position;
		};

		auto get_rotation = [](RigidBodyComponent* body) -> Math::quat {
			Math::vec3 position;
			Math::quat orientation;
			body->ptr->GetTransform(position, orientation);
			return orientation;
		};

		auto set_translation = [](RigidBodyComponent* body, const Math::vec3& translation) {
			Math::vec3 position;
			Math::quat orientation;
			body->ptr->GetTransform(position, orientation);
			body->ptr->SetTransform(translation, orientation);
		};

		auto set_rotation = [](RigidBodyComponent* body, const Math::quat& rotation) {
			Math::vec3 position;
			Math::quat orientation;
			body->ptr->GetTransform(position, orientation);
			body->ptr->SetTransform(position, rotation);
		};

		auto set_linear_momentum = [](RigidBodyComponent* body, const Math::vec3& velocity) {
			body->ptr->SetLinearMomentum(velocity);
		};

		auto set_angular_momentum  = [](RigidBodyComponent* body, const Math::vec3& velocity) {
			body->ptr->SetAngularMomentum(velocity);
		};

		state.new_usertype<RigidBodyComponent>(
			"CollisionComponent",
			sol::no_constructor,
			"GetTranslation", get_translation,
			"GetRotation", get_rotation,
			"SetTranslation", set_translation,
			"SetRotation", set_rotation,
			"SetLinearMomentum", set_linear_momentum,
			"SetAngularMomentum", set_angular_momentum
		);
	}

	void RegisterAnimationComponent(sol::state& state)
	{
		auto set_animation = [](SkinnedRenderableComponent* anim, const std::string& name) {
			anim->animator.Load(*AssetManager<Animation>::Instance().Get(name));
		};

		auto get_duration = [](SkinnedRenderableComponent* anim) -> float {
			return anim->animator.GetDuration();
		};

		auto get_material_string = [](SkinnedRenderableComponent* anim, const int index) -> std::string {
			return anim->model->GetMaterial(index);
		};

		auto get_mesh_material_count = [](SkinnedRenderableComponent* anim) -> int {
			return anim->model->GetMeshCount();
		};

		auto set_material_color = [](SkinnedRenderableComponent* anim, const std::string& id, const Math::vec4& color) {
			AssetManager<Material>::Instance().Get(id)->SetColor(color);
		};

		auto pause = [](SkinnedRenderableComponent* anim) {
			anim->animator.Pause();
		};

		auto play = [](SkinnedRenderableComponent* anim) {
			anim->animator.Play();
		};

		state.new_usertype<SkinnedRenderableComponent>(
			"AnimationComponent",
			sol::no_constructor,
			"SetAnimation", set_animation,
			"GetDuration", get_duration,
			"GetMaterialString", get_material_string,
			"GetMeshMaterialCount", get_mesh_material_count,
			"SetMaterialColor", set_material_color,
			"Play", play,
			"Pause", pause
		);
	}

	void RegisterTagComponent(sol::state& state)
	{
		state.new_usertype<TagComponent>(
			"TagComponent",
			sol::constructors<
				TagComponent(),
				TagComponent(const std::string&, Uint16)
			>(),
			"tag", &TagComponent::tag,
			"ident", &TagComponent::ident
		);
	}

	void RegisterNavigationGridComponent(sol::state& state)
	{
		auto get_waypoints = [](NavigationGridComponent* nav, const Math::vec3 startPos, const Math::vec3 endPos, bool check_neighbours) -> std::vector<float> {
			return nav->grid->GetAStarPath(startPos, endPos, check_neighbours);
		};

		auto add_obstacle = [](NavigationGridComponent* nav, const Math::vec3& pos, const std::string& id, int radius) {
			nav->grid->AddObstacle(pos, id, radius);
		};

		auto toggle_obstacle = [](NavigationGridComponent* nav, const std::string& id, bool enable) {
			nav->grid->ToggleObstacle(id, enable);
		};

		state.new_usertype<NavigationGridComponent>(
			"NavigationGridComponent",
			sol::constructors<NavigationGridComponent()>(),
			"GetWaypoints", get_waypoints,
			"AddObstacle", add_obstacle,
			"ToggleObstacle", toggle_obstacle
		);
	}

	void RegisterCanvasComponent(sol::state& state)
	{
		state.new_usertype<CanvasRendererComponent>(
			"CanvasRendererComponent",
			sol::constructors<CanvasRendererComponent()>(),
			"active", &CanvasRendererComponent::active,
			"screenspace", &CanvasRendererComponent::screenSpace
		);
	}

	void RegisterPanelComponent(sol::state& state)
	{
		auto set_texture = [](PanelComponent* panel, const std::string& name) {
			panel->texture = AssetManager<Texture>::Instance().Get(name);
		};

		state.new_usertype<PanelComponent>(
			"PanelComponent",
			sol::no_constructor,
			"SetTexture", set_texture
		);
	}

	void RegisterPlayerControllerComponent(sol::state &state)
	{
		auto move = [](PlayerControllerComponent& controller, Math::vec3& force) {
			controller.ptr->ApplyForce(force);
		};

		auto set_speed = [](PlayerControllerComponent& controller, float speed) {
			controller.ptr->SetSpeed(speed);
		};

		auto get_speed = [](PlayerControllerComponent& controller) -> float {
			return controller.ptr->GetControllerProperties().moveFactor;
		};

		auto get_unit_direction = [](PlayerControllerComponent& controller) -> Math::vec3 {
			return controller.ptr->GetUnitDirection();
		};

		state.new_usertype<PlayerControllerComponent>(
			"PlayerControllerComponent",
			sol::no_constructor,
			"Move", move,
			"SetSpeed", set_speed,
			"GetSpeed", get_speed,
			"GetUnitDirection", get_unit_direction
		);
	}

	// void RegisterScriptableComponent(sol::state& state)
	// {
	// 	auto addScript = [](ScriptableComponent* scriptable, const std::string& ident) {
	// 		scriptable->script = MakeUnique<EntityScript>(ScriptEngineImpl::Instance().GetState(), ident);
	// 	};

	// 	state.new_usertype<ScriptableComponent>(
	// 		"ScriptableComponent",
	// 		sol::constructors<ScriptableComponent()>(),
	// 		"AddScript", addScript,
	// 		"AssignEntity", &ScriptableComponent::SetEntity
	// 	);
	// }

	void RegisterTransformComponent(sol::state& state)
	{
		state.new_usertype<TransformComponent>(
			"TransformComponent",
			sol::constructors<TransformComponent()>(),
			"translation", &TransformComponent::translation,
			"orientation", &TransformComponent::orientation,
			"scale", &TransformComponent::scale,
			"LocalX", &TransformComponent::GetLocalX,
			"LocalY", &TransformComponent::GetLocalY,
			"LocalZ", &TransformComponent::GetLocalZ,
			"LookAt", &TransformComponent::LookAt
		);
	}

	void RegisterRectTransformComponent(sol::state& state)
	{
		state.new_usertype<RectTransformComponent>(
			"RectTransformComponent",
			sol::constructors<RectTransformComponent()>(),
			"translation", &RectTransformComponent::translation,
			"orientation", &RectTransformComponent::orientation,
			"scale", &RectTransformComponent::scale
		);
	}

	void RegisterRenderableComponent(sol::state& state)
	{
		auto setModel = [](RenderableComponent* renderable, const std::string& ident) {
			renderable->model = AssetManager<Model>::Instance().Get(ident);
		};

		auto SetShader = [](RenderableComponent* renderable, const std::string& ident) {
			renderable->shader = AssetManager<Shader>::Instance().Get(ident);
		};

		state.new_usertype<RenderableComponent>(
			"RenderableComponent",
			sol::constructors<RenderableComponent()>(),
			"active", &RenderableComponent::active,
			"SetModel", setModel,
			"SetShader", SetShader
		);
	}

	void RegisterTerrainComponent(sol::state& state)
	{
		/// \todo Implement RegisterTerrainComponent
	}

	void RegisterCameraComponent(sol::state& state)
	{
		state.new_usertype<CameraComponent>(
			"CameraComponent",
			sol::constructors<CameraComponent()>(),
			"camera", &CameraComponent::camera
		);
	}

	void RegisterBDIAgent(sol::state &state)
	{
		state.new_usertype<BDIAgent>(
			"BDIAgent",
			sol::constructors<
				BDIAgent(const std::string&)
			>(),
			"OnUpdate", &BDIAgent::OnUpdate,
			"GetName", &BDIAgent::GetName,
			"SetActivationLevel", &BDIAgent::SetActivationLevel,
			"GetActivationLevel", &BDIAgent::GetActivationLevel,
			"SetIntentionThreshold", &BDIAgent::SetIntentionThreshold,
			"GetIntentionThreshold", &BDIAgent::GetIntentionThreshold,
			"AddBelief", &BDIAgent::AddBelief,
			"RemoveBelief", &BDIAgent::RemoveBelief,
			"AddDesire", &BDIAgent::AddDesire,
			"RemoveDesire", &BDIAgent::RemoveDesire,
			"AddIntention", &BDIAgent::AddIntention,
			"RemoveIntention", &BDIAgent::RemoveIntention
		);
	}

	void RegisterBDIComponent(sol::state &state)
	{
		auto get_agent = [](BDIComponent* bdi) -> BDIAgent* {
			return bdi->ptr.get();
		};

		state.new_usertype<BDIComponent>(
			"BDIComponent",
			sol::no_constructor,
			"GetAgent", get_agent
		);
	}

	void RegisterFCM(sol::state &state)
	{
		auto get_concept_value = [](FCM* fcm, unsigned int index) -> float {
			return fcm->GetConceptValue(index);
		};

		auto set_concept_value = [](FCM* fcm, unsigned int index, float value) -> bool {
			return fcm->SetConceptValue(index, value);
		};

		state.new_usertype<FCM>(
			"FCM",
			sol::constructors<
				FCM()
			>(),
			"Init", &FCM::Init,
			"OnUpdate", &FCM::OnUpdate,
			"AddConcept", &FCM::AddConcept,
			"AddEdge", &FCM::AddEdge,
			"GetConceptValue", get_concept_value,
			"SetConceptValue", set_concept_value
		);
	}

	void RegisterFCMComponent(sol::state &state)
	{
		auto get_fcm = [](FCMComponent* fcm) -> FCM* {
			return fcm->ptr.get();
		};

		state.new_usertype<FCMComponent>(
			"FCMComponent",
			sol::no_constructor,
			"GetFCM", get_fcm
		);
	}

	void RegisterEntityModule(sol::state &state)
	{
		RegisterEntity(state);
		RegisterTagComponent(state);
		RegisterTransformComponent(state);
		RegisterCanvasComponent(state);
		RegisterNavigationGridComponent(state);
		RegisterPanelComponent(state);
		RegisterRectTransformComponent(state);
		RegisterRenderableComponent(state);
		RegisterTerrainComponent(state);
		RegisterCameraComponent(state);
		RegisterPlayerControllerComponent(state);
		RegisterAnimationComponent(state);
		RegisterCollisionBody(state);
		RegisterBDIAgent(state);
		RegisterBDIComponent(state);
		RegisterFCM(state);
		RegisterFCMComponent(state);
	}

//--------------------------------------------------------------------------------
// FSM Module
//--------------------------------------------------------------------------------
	void RegisterFSMState(sol::state& state)
	{
		state.new_usertype<FSMState>(
			"FSMState",
			sol::constructors<
				FSMState(),
				FSMState(
					const std::string&,
					std::set<int>,
					std::function<int(float)>
				),
				FSMState(
					const std::string&,
					std::set<int>,
					std::function<int(float)>,
					std::function<void()>
				),
				FSMState(
					const std::string&,
					std::set<int>,
					std::function<int(float)>,
					std::function<void()>,
					std::function<void()>
				)
			>(),
			"HasTransition", &FSMState::HasTransition,
			"GetName", &FSMState::GetName
		);
	}

	void RegisterFSM(sol::state& state)
	{
		auto onUpdate = [](FSM* fsm, float deltaTime) {
			fsm->OnUpdate(TimeStep{deltaTime});
		};

		state.new_usertype<FSM>(
			"FSM",
			sol::constructors<
				FSM(std::vector<FSMState>),
				FSM(std::vector<FSMState>, int)
			>(),
			"Init", &FSM::Init,
			"OnUpdate", onUpdate,
			"GoToState", &FSM::GoToState,
			"GetCurrentState", &FSM::GetCurrentState
		);
	}

	void RegisterFSMModule(sol::state& state)
	{
		RegisterFSMState(state);
		RegisterFSM(state);
	}

//--------------------------------------------------------------------------------
// Messaging System
//--------------------------------------------------------------------------------
	void RegisterMessageService(sol::state& state)
	{
		state.new_usertype<MessageService>(
			"MessageService",
			sol::no_constructor,
			"CreateAgent", &MessageService::CreateAgent
		);
	}

	void RegisterMessage(sol::state& state)
	{
		state.new_usertype<Message>(
			"Message",
			sol::no_constructor,
			"sender", &Message::sender,
			"receiver", &Message::receiver,
			"type", &Message::type,
			"payload", &Message::payload
		);
	}

	void RegisterMessageAgent(sol::state& state)
	{
		state.new_usertype<MessageAgent>(
			"MessageAgent",
			sol::no_constructor,

			// destroy agent
			"Destroy", &MessageAgent::Destroy,

			// configuration
			"AddToCategory", sol::overload(
				static_cast<void (MessageAgent::*)(AgentCategory)>(&MessageAgent::AddToCategory),
				static_cast<void (MessageAgent::*)(AgentCategorySet)>(&MessageAgent::AddToCategory)
			),
			"RemoveFromCategory", sol::overload(
				static_cast<void (MessageAgent::*)(AgentCategory)>(&MessageAgent::RemoveFromCategory),
				static_cast<void (MessageAgent::*)(AgentCategorySet)>(&MessageAgent::RemoveFromCategory)
			),
			"RegisterMessageHandler", &MessageAgent::RegisterMessageHandler,
			"UnregisterMessageHandler", &MessageAgent::UnregisterMessageHandler,

			// message sending
			"BroadcastMessage", &MessageAgent::BroadcastMessage,
			"SendMessageToAgent", sol::overload(
				static_cast<void (MessageAgent::*)(Agent, MessageType, MessageData)>(&MessageAgent::SendMessageToAgent),
				static_cast<void (MessageAgent::*)(AgentSet, MessageType, MessageData)>(&MessageAgent::SendMessageToAgent)
			),
			"SendMessageToCategory", sol::overload(
				static_cast<void (MessageAgent::*)(AgentCategory, MessageType, MessageData)>(&MessageAgent::SendMessageToCategory),
				static_cast<void (MessageAgent::*)(AgentCategorySet, MessageType, MessageData)>(&MessageAgent::SendMessageToCategory)
			),

			// introspection
			"GetRegisteredCategories", &MessageAgent::GetRegisteredCategories,
			"GetRegisteredMessageTypes", &MessageAgent::GetRegisteredMessageTypes
		);
	}

	void RegisterMessagingSystem(sol::state& state)
	{
		RegisterMessageService(state);
		RegisterMessage(state);
		RegisterMessageAgent(state);
	}

//--------------------------------------------------------------------------------
// Initialisation
//--------------------------------------------------------------------------------
	void ScriptEngineImpl::Init()
	{
		if (m_isInitialized)
		{
			return;
		}

		// caputure reference to internal sol state
		sol::state& solState = m_state.GetNative();
		RegisterInputModule(solState);
		RegisterMathModule(solState);
		RegisterTypesModule(solState);
		RegisterCoreModule(solState);
		RegisterSceneModule(solState);
		RegisterEntityModule(solState);
		RegisterFSMModule(solState);
		RegisterMessagingSystem(solState);
		m_isInitialized = true;
	}
}
