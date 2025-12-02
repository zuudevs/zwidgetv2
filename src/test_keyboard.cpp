#include "zwidget/core/application.hpp"
#include "zwidget/zwidget.hpp"
#include <print>
#include <synchapi.h>

using namespace zuu::widget ;

int main() {
	if (!Renderer::initialize_factories()) {
		std::println("Failed to initialize D2D") ;
		return 1 ;
	}

	if (!Application::initialize("TestKeyboard")) {
		std::println("Failed to initialize application") ;
		return 1 ;
	}

	Window window("Test Keyboard Input", Size(800, 600)) ;
	window.show() ;

	while (Application::is_running()) {
		Event ev ;
		while (EventDispatcher::PollEvent(ev)) {
			if (auto* e = ev.get_if<WindowEvent>()) {
				if (e->get_type() == WindowEvent::Type::close) {
					window.close() ;
					Application::shutdown() ;
				}
			} else if (auto* e = ev.get_if<KeyboardEvent>()) {
				if (e->get_type() == KeyboardEvent::Type::key_press) {
					std::println("Key Pressed: {}", static_cast<char>(e->get_key())) ;
				} else if (e->get_type() == KeyboardEvent::Type::key_release) {
					std::println("Key Released: {}", static_cast<char>(e->get_key())) ;
				}
			}
		}

		Sleep(16) ;
	}
}