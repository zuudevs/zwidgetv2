#pragma once

#include "zwidget/core/event_dispatcher.hpp"

namespace zuu::widget {

	class Window {
	private :
		HWND hwnd ;

	public:
		Window() noexcept = delete ;
		Window(const Window&) noexcept = delete ;
		Window(Window&&) noexcept = default ;
		Window& operator=(const Window&) noexcept = delete ;
		Window& operator=(Window&&) noexcept = default ;
		
		void show() noexcept {
			ShowWindow(hwnd, SW_SHOW) ;
		}
	} ;

} // namespace zuu::widget