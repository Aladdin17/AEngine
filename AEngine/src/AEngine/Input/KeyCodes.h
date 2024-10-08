/**
 * \file
 * \author Ben Hawkins (34112619)
 * \author Christien Alden (34119981)
 * \brief Keys and mouse button codes used within AEngine
**/
#pragma once

enum class AEInputState
{
	Released,   ///< The input state is released
	Pressed,    ///< The input state is pressed
	Repeated,   ///< The input state is repeated
	Unknown     ///< Returned when the key/mouse button is invalid
};


	/**
	 * \enum AEKey
	 * \brief Keys codes used within AEngine
	*/
enum class AEKey
{
	// Common keys
	SPACE,
	APOSTROPHE,
	COMMA,
	MINUS,
	PERIOD,
	SLASH,
	NUM0,
	NUM1,
	NUM2,
	NUM3,
	NUM4,
	NUM5,
	NUM6,
	NUM7,
	NUM8,
	NUM9,
	SEMICOLON,
	EQUAL,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	LEFT_BRACKET,
	BACKSLASH,
	RIGHT_BRACKET,
	GRAVE_ACCENT,

	// Function keys
	ESCAPE,
	ENTER,
	TAB,
	BACKSPACE,
	INSERT,
	DEL,
	RIGHT,
	LEFT,
	DOWN,
	UP,
	PAGE_UP,
	PAGE_DOWN,
	HOME,
	END,
	CAPS_LOCK,
	SCROLL_LOCK,
	NUM_LOCK,
	PRINT_SCREEN,
	PAUSE,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,
	F25,
	KP_0,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,
	KP_DECIMAL,
	KP_DIVIDE,
	KP_MULTIPLY,
	KP_SUBTRACT,
	KP_ADD,
	KP_ENTER,
	KP_EQUAL,
	LEFT_SHIFT,
	LEFT_CONTROL,
	LEFT_ALT,
	RIGHT_SHIFT,
	RIGHT_CONTROL,
	RIGHT_ALT,

	INVALID
};

	/**
	 * \enum AEMouse
	 * \brief Mouse button codes used within AEngine
	*/
enum class AEMouse
{
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8,
	INVALID = BUTTON_8,
	BUTTON_LEFT = BUTTON_1,
	BUTTON_RIGHT = BUTTON_2,
	BUTTON_MIDDLE = BUTTON_3
};
