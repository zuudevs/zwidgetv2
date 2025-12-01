#pragma once

#include "zwidget/unit/point.hpp"
#include "keymod.hpp"

namespace zuu::widget {

	class MouseEvent {
	public :
		enum class Type : uint8_t {
			none,
			scroll,
			move,
			button_press,
			button_release,
		} ;

		enum class Button : uint8_t {
			none,
			left,
			middle,
			right,
			x1,
			x2,
		} ;

	private :
		Type type {} ;						// 	1 byte
		Button button {} ;					// 	1 byte
		KeyMod key_mods {} ;				//	1 byte
		uint8_t padding[1] {} ;				// 	1 byte
		int16_t scroll_delta {} ;			// 	2 byte
		basic_point<uint16_t> position {} ;	// 	4 byte
		// ------------------------------------------- +
		// total :								10 bytes

	public :
		constexpr MouseEvent() noexcept = default ;
		constexpr MouseEvent(const MouseEvent&) noexcept = default ;
		constexpr MouseEvent(MouseEvent&&) noexcept = default ;
		constexpr MouseEvent& operator=(const MouseEvent&) noexcept = default ;
		constexpr MouseEvent& operator=(MouseEvent&&) noexcept = default ;
		constexpr bool operator==(const MouseEvent&) const noexcept = default ;
		constexpr bool operator!=(const MouseEvent&) const noexcept = default ;
		constexpr ~MouseEvent() noexcept = default ;

		constexpr MouseEvent(Type type_, const basic_point<uint16_t>& position_) noexcept
		 : type(type_), position(position_) {}

		constexpr MouseEvent(Type type_, uint16_t scroll_delta) noexcept
		 : type(type_), scroll_delta(scroll_delta) {}

		constexpr MouseEvent(
			Type type_,
			Button button_,
			const basic_point<uint16_t>& position_
		) noexcept
		 : type(type_), button(button_), position(position_) {}

		constexpr Type get_type() const noexcept {
			return type ;
		}

		constexpr Button get_button() const noexcept {
			return button ;
		}

		constexpr const basic_point<uint16_t>& get_position() const noexcept {
			return position ;
		}

		constexpr const uint16_t& get_scroll_delta() const noexcept {
			return scroll_delta ;
		}

		constexpr const KeyMod& get_key_mods() const noexcept {
			return key_mods ;
		}
	} ;

} // namespace zuu::widget