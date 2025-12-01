#pragma once

#include <cstdint>

namespace zuu::widget {

	class KeyboardEvent {
	public :
		enum class Type : uint8_t {
			none,
			key_press,
			key_release,
			text_input,
		} ;

		enum class KeyCode : uint16_t {
		// CONTROL & NAVIGATION
		none		= 0,
		unknown 	= 0,
		Back		= 8,
		Tab			= 9,
		Enter		= 13,
		Shift		= 16,
		Control		= 17,
		Alt			= 18,    // Alt
		Pause		= 19,
		CapsLock	= 20,
		Escape		= 27,
		Space		= 32,
		PageUp		= 33,
		PageDown	= 34,
		End			= 35,
		Home		= 36,
		Left		= 37,
		Up			= 38,
		Right		= 39,
		Down		= 40,
		Insert		= 45,
		Delete		= 46,

		// NUMBER KEYS (TOP)
		Number0 = 48,
		Number1 = 49,
		Number2 = 50,
		Number3 = 51,
		Number4 = 52,
		Number5 = 53,
		Number6 = 54,
		Number7 = 55,
		Number8 = 56,
		Number9 = 57,

		// LETTER KEYS (A–Z)
		A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72,
		I = 73, J = 74, K = 75, L = 76, M = 77, N = 78, O = 79, P = 80,
		Q = 81, R = 82, S = 83, T = 84, U = 85, V = 86, W = 87, X = 88,
		Y = 89, Z = 90,

		// WINDOWS & MENU
		LeftWindows  = 91,
		RightWindows = 92,
		Application  = 93,

		// NUMPAD SECTION
		NumPad0 = 96,
		NumPad1 = 97,
		NumPad2 = 98,
		NumPad3 = 99,
		NumPad4 = 100,
		NumPad5 = 101,
		NumPad6 = 102,
		NumPad7 = 103,
		NumPad8 = 104,
		NumPad9 = 105,

		Multiply	= 106,
		Add			= 107,
		Separator	= 108, // Enter (numeric)
		Subtract	= 109,
		Decimal		= 110,
		Divide		= 111,
		NumLock		= 144,

		// FUNCTION KEYS
		F1 = 112, F2 = 113, F3 = 114, F4 = 115, F5 = 116, F6 = 117,
		F7 = 118, F8 = 119, F9 = 120, F10 = 121, F11 = 122, F12 = 123,
		F13 = 124, F14 = 125, F15 = 126, F16 = 127, F17 = 128, F18 = 129,
		F19 = 130, F20 = 131, F21 = 132, F22 = 133, F23 = 134, F24 = 135,

		// MODIFIER KEYS
		LeftShift		= 160,
		RightShift		= 161,
		LeftControl		= 162,
		RightControl	= 163,
		LeftAlt			= 164,
		RightAlt		= 165,

		// GAMEPAD / CONTROLLER
		GamepadA						= 195,
		GamepadB						= 196,
		GamepadX						= 197,
		GamepadY						= 198,
		GamepadRightShoulder			= 199,
		GamepadLeftShoulder				= 200,
		GamepadLeftTrigger				= 201,
		GamepadRightTrigger				= 202,
		GamepadDPadUp					= 203,
		GamepadDPadDown					= 204,
		GamepadDPadLeft					= 205,
		GamepadDPadRight				= 206,
		GamepadMenu						= 207,
		GamepadView						= 208,
		GamepadLeftThumbstickButton		= 209,
		GamepadRightThumbstickButton	= 210,
		GamepadLeftThumbstickUp			= 211,
		GamepadLeftThumbstickDown		= 212,
		GamepadLeftThumbstickRight		= 213,
		GamepadLeftThumbstickLeft		= 214,
		GamepadRightThumbstickUp		= 215,
		GamepadRightThumbstickDown		= 216,
		GamepadRightThumbstickRight		= 217,
		GamepadRightThumbstickLeft		= 218
	} ;

	private :
		Type type = Type::none ;			//  1 byte
		uint8_t padding[1] {} ;				//  1 byte
		KeyCode key = KeyCode::unknown ;	//  2 byte
		// --------------------------------------------
		// Total : 								6 bytes

	public :
		constexpr KeyboardEvent() noexcept = default ;
		constexpr KeyboardEvent(const KeyboardEvent&) noexcept = default ;
		constexpr KeyboardEvent(KeyboardEvent&&) noexcept = default ;
		constexpr KeyboardEvent& operator=(const KeyboardEvent&) noexcept = default ;
		constexpr KeyboardEvent& operator=(KeyboardEvent&&) noexcept = default ;
		constexpr bool operator==(const KeyboardEvent&) const noexcept = default ;
		constexpr bool operator!=(const KeyboardEvent&) const noexcept = default ;
		constexpr ~KeyboardEvent() noexcept = default ;

		explicit KeyboardEvent(Type type, KeyCode key = KeyCode::unknown)
			: type(type), key(key) {}

		Type get_type() const {
			return type ;
		}

		KeyCode get_key() const {
			return key ;
		}

		wchar_t get_character() const {
			return static_cast<wchar_t>(key) ;
		}
	} ;

} // namespace zuu::widget