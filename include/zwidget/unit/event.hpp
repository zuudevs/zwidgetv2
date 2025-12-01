#pragma once

#include "events/window.hpp"
#include "events/mouse.hpp"
#include "events/keyboard.hpp"
#include "zwidget/detail/variant.hpp"
#include <stdexcept>

#include <Windows.h>
#include <Windowsx.h>

#if defined (max) && defined (min)
	#undef max
	#undef min
#endif

namespace zuu::widget {
	class Event {
	public :
		enum class Type : uint8_t {
			none,
			window,
			mouse,
			keyboard,
			Quit,
		} ;

		class EmptyEvent {} ;

		using events_t = std::variant<EmptyEvent, WindowEvent, MouseEvent, KeyboardEvent> ;

	private :
		HWND hwnd_ {} ;
		Type type_ = Type::none ;
		events_t event {} ;

		constexpr Event (HWND hwnd, Type type, events_t event) noexcept
		 : hwnd_(hwnd), type_(type), event(event) {}

	public :
		constexpr Event() noexcept = default ;
		constexpr Event(const Event&) noexcept = default ;
		constexpr Event(Event&&) noexcept = default ;
		constexpr Event& operator=(const Event&) noexcept = default ;
		constexpr Event& operator=(Event&&) noexcept = default ;
		constexpr ~Event() noexcept = default ;

		static constexpr Event create_window_event (HWND hwnd, const WindowEvent& event) noexcept {
			if (event.get_type() < WindowEvent::Type::resize && !event.get_size().is_empty()) 
				return Event{} ;
			return Event(hwnd, Type::window, event) ;
		}

		static constexpr Event create_mouse_event (HWND hwnd, const MouseEvent& event) noexcept {
			if (event.get_type() > MouseEvent::Type::scroll && !event.get_position().is_empty()) 
				return Event{} ;
			return Event(hwnd, Type::mouse, event) ;
		}

		static constexpr Event create_keyboard_event (HWND hwnd, const KeyboardEvent& event) noexcept {
			return Event(hwnd, Type::keyboard, event) ;
		}

		static constexpr Event create_quit_event() noexcept {
			return Event(nullptr, Type::Quit, EmptyEvent{}) ;
		}

		template <meta::variant_type<events_t> T>
		[[nodiscard]] constexpr const T* get_if() const noexcept {
			return std::get_if<T>(&event) ;
		}

		template <meta::variant_type<events_t> T>
		[[nodiscard]] constexpr T* get_if() noexcept {
			return std::get_if<T>(&event) ;
		}

		template <meta::variant_type<events_t> T>
		constexpr bool is_type() const noexcept {
			return std::holds_alternative<T>(event) ;
		}
	} ;
}