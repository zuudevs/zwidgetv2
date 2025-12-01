#pragma once

#include "zwidget/unit/event.hpp"

namespace zuu::detail {
	[[nodiscard]] inline widget::Event CreateEventFromMSG(const MSG& msg) noexcept {
		switch (msg.message) {
			case WM_CLOSE : return widget::Event::create_window_event(
				msg.hwnd,
				widget::WindowEvent(widget::WindowEvent::Type::close)
			) ;
			case WM_SIZE : {
				widget::WindowEvent::Type type ;
				switch (msg.wParam) {
					case SIZE_MINIMIZED :
						type = widget::WindowEvent::Type::minimize ;
						break ;
					case SIZE_MAXIMIZED :
						type = widget::WindowEvent::Type::maximize ;
						break ;
					case SIZE_RESTORED :
					default :
						type = widget::WindowEvent::Type::restored ;
						break ;
				}
				return widget::Event::create_window_event(
					msg.hwnd,
					widget::WindowEvent(
						type,
						widget::basic_size<uint16_t>(
							static_cast<uint16_t>(LOWORD(msg.lParam)),
							static_cast<uint16_t>(HIWORD(msg.lParam))
						)
					)
				) ;
			}

			case WM_MOUSEMOVE : {
				return widget::Event::create_mouse_event(
					msg.hwnd,
					widget::MouseEvent(
						widget::MouseEvent::Type::move,
						widget::basic_point<uint16_t>(
							static_cast<uint16_t>(GET_X_LPARAM(msg.lParam)),
							static_cast<uint16_t>(GET_Y_LPARAM(msg.lParam))
						)
					)
				) ;
			}

			case WM_MOUSEWHEEL : {
				return widget::Event::create_mouse_event(
					msg.hwnd,
					widget::MouseEvent(
						widget::MouseEvent::Type::scroll,
						static_cast<uint16_t>(GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA)
					)
				) ;
			}

			case WM_LBUTTONDOWN :
			case WM_MBUTTONDOWN :
			case WM_RBUTTONDOWN :
			case WM_XBUTTONDOWN : {
				widget::MouseEvent::Button button ;
				switch (msg.message) {
					case WM_LBUTTONDOWN : button = widget::MouseEvent::Button::left ; break ;
					case WM_MBUTTONDOWN : button = widget::MouseEvent::Button::middle ; break ;
					case WM_RBUTTONDOWN : button = widget::MouseEvent::Button::right ; break ;
					case WM_XBUTTONDOWN :
						button = (HIWORD(msg.wParam) == XBUTTON1)
						 ? widget::MouseEvent::Button::x1
						 : widget::MouseEvent::Button::x2 ;
						break ;
					default : button = widget::MouseEvent::Button::none ; break ;
				}
				return widget::Event::create_mouse_event(
					msg.hwnd,
					widget::MouseEvent(
						widget::MouseEvent::Type::button_press,
						button,
						widget::basic_point<uint16_t>(
							static_cast<uint16_t>(GET_X_LPARAM(msg.lParam)),
							static_cast<uint16_t>(GET_Y_LPARAM(msg.lParam))
						)
					)
				) ;
			}

			case WM_LBUTTONUP :
			case WM_MBUTTONUP :
			case WM_RBUTTONUP :
			case WM_XBUTTONUP : {
				widget::MouseEvent::Button button ;
				switch (msg.message) {
					case WM_LBUTTONUP : button = widget::MouseEvent::Button::left ; break ;
					case WM_MBUTTONUP : button = widget::MouseEvent::Button::middle ; break ;
					case WM_RBUTTONUP : button = widget::MouseEvent::Button::right ; break ;
					case WM_XBUTTONUP :
						button = (HIWORD(msg.wParam) == XBUTTON1)
						 ? widget::MouseEvent::Button::x1
						 : widget::MouseEvent::Button::x2 ;
						break ;
					default : button = widget::MouseEvent::Button::none ; break ;
				}
				return widget::Event::create_mouse_event(
					msg.hwnd,
					widget::MouseEvent(
						widget::MouseEvent::Type::button_release,
						button,
						widget::basic_point<uint16_t>(
							static_cast<uint16_t>(GET_X_LPARAM(msg.lParam)),
							static_cast<uint16_t>(GET_Y_LPARAM(msg.lParam))
						)
					)
				) ;
			}

			case WM_KEYDOWN :
			case WM_SYSKEYDOWN :
			case WM_KEYUP :
			case WM_SYSKEYUP : {
				bool is_press = (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) ;
				return widget::Event::create_keyboard_event(
					msg.hwnd,
					widget::KeyboardEvent(
						is_press
						 ? widget::KeyboardEvent::Type::key_press
						 : widget::KeyboardEvent::Type::key_release,
						static_cast<widget::KeyboardEvent::KeyCode>(msg.wParam)
					)
				) ;
			}

			case WM_QUIT : return widget::Event::create_quit_event() ;
			default : return widget::Event{} ;
		}
	}
}