#include "zwidget/unit/window.hpp"
#include "zwidget/core/application.hpp"
#include <print>
#include <format>
#include <vector>
#include <memory>

using namespace zuu::widget;

// Helper: Print event info
void print_event(const Event& event, const std::string& window_name) {
    if (auto* we = event.get_if<WindowEvent>()) {
        switch (we->get_type()) {
            case WindowEvent::Type::close:
                std::println("[{}] 📪 Window CLOSE", window_name);
                break;
            case WindowEvent::Type::focus_gained:
                std::println("[{}] 🎯 Focus GAINED", window_name);
                break;
            case WindowEvent::Type::focus_lost:
                std::println("[{}] 😶‍🌫️ Focus LOST", window_name);
                break;
            case WindowEvent::Type::resize:
                std::println("[{}] 📏 RESIZE: {}x{}", window_name, 
                           we->get_size().w, we->get_size().h);
                break;
            case WindowEvent::Type::minimize:
                std::println("[{}] 🔽 MINIMIZED", window_name);
                break;
            case WindowEvent::Type::maximize:
                std::println("[{}] 🔼 MAXIMIZED", window_name);
                break;
            case WindowEvent::Type::restored:
                std::println("[{}] ↩️ RESTORED", window_name);
                break;
        }
    }
    else if (auto* me = event.get_if<MouseEvent>()) {
        if (me->get_type() == MouseEvent::Type::button_press) {
            const char* btn = "UNKNOWN";
            switch (me->get_button()) {
                case MouseEvent::Button::left: btn = "LEFT"; break;
                case MouseEvent::Button::right: btn = "RIGHT"; break;
                case MouseEvent::Button::middle: btn = "MIDDLE"; break;
                default: break;
            }
            std::println("[{}] 🖱️ Mouse {} at ({}, {})", 
                       window_name, btn, me->get_position().x, me->get_position().y);
        }
    }
    else if (auto* ke = event.get_if<KeyboardEvent>()) {
        if (ke->get_type() == KeyboardEvent::Type::key_press) {
            std::string key = "KEY_" + std::to_string(static_cast<int>(ke->get_key()));
            if (ke->get_key() == KeyboardEvent::KeyCode::Escape) {
                key = "ESC";
            } else if (ke->get_key() >= KeyboardEvent::KeyCode::A && 
                      ke->get_key() <= KeyboardEvent::KeyCode::Z) {
                key = std::string(1, static_cast<char>(ke->get_key()));
            }
            std::println("[{}] ⌨️ Key pressed: {}", window_name, key);
        }
    }
}

// Window wrapper untuk tracking
struct WindowInfo {
    std::unique_ptr<Window> window;
    std::string name;
    
    WindowInfo(std::unique_ptr<Window> w, std::string n)
        : window(std::move(w)), name(std::move(n)) {}
};

int main() {
    try {
        std::println("\n=== ZWidget Multi-Window Application ===\n");

        // Initialize Application
        if (!Application::initialize("ZWidgetMultiWindowApp")) {
            std::println("❌ Failed to initialize application");
            return 1;
        }
        std::println("✅ Application initialized\n");

        // Create multiple windows
        std::vector<WindowInfo> windows;

        // Main window
        windows.emplace_back(
            std::make_unique<Window>(
                "Main Window",
                Size(800, 600),
                WindowStyle::Default
            ),
            "Main"
        );
        windows.back().window->show();
        std::println("✅ Created: Main Window (800x600)");

        // Child window 1
        windows.emplace_back(
            std::make_unique<Window>(
                "Child Window 1",
                Point(100, 100),
                Size(400, 300),
                WindowStyle::Default
            ),
            "Child1"
        );
        windows.back().window->show();
        std::println("✅ Created: Child Window 1 (400x300)");

        // Child window 2
        windows.emplace_back(
            std::make_unique<Window>(
                "Child Window 2",
                Point(600, 100),
                Size(400, 300),
                WindowStyle::Default
            ),
            "Child2"
        );
        windows.back().window->show();
        std::println("✅ Created: Child Window 2 (400x300)");

        std::println("\n📊 Active windows: {}", Application::window_count());
        std::println("\n📝 Instructions:");
        std::println("   - Click on windows to switch focus");
        std::println("   - Press ESC in any window to close it");
        std::println("   - Close all windows to exit");
        std::println("   - Try minimize/maximize/resize\n");

        // Event loop
        std::println("=== Event Loop Started ===\n");
        
        while (Application::is_running()) {
            Event event;
            
            // Poll all events
            while (EventDispatcher::PollEvent(event)) {
                // Check for quit
                if (event.is_quit_event()) {
                    std::println("\n🚪 QUIT EVENT - Application exiting");
                    break;
                }

                // Find which window this event belongs to
                HWND event_hwnd = event.get_hwnd();
                std::string window_name = "Unknown";
                
                for (auto& info : windows) {
                    if (info.window->get_handle() == event_hwnd) {
                        window_name = info.name;
                        break;
                    }
                }

                // Print event
                print_event(event, window_name);

                // Handle window close events
                if (auto* we = event.get_if<WindowEvent>()) {
                    if (we->get_type() == WindowEvent::Type::close) {
                        // Find and close window
                        for (auto it = windows.begin(); it != windows.end(); ++it) {
                            if (it->window->get_handle() == event_hwnd) {
                                std::println("   → Closing window: {}", it->name);
                                it->window->close();
                                windows.erase(it);
                                std::println("   → Remaining windows: {}", windows.size());
                                break;
                            }
                        }
                    }
                }

                // Handle ESC key - close window
                if (auto* ke = event.get_if<KeyboardEvent>()) {
                    if (ke->get_type() == KeyboardEvent::Type::key_press &&
                        ke->get_key() == KeyboardEvent::KeyCode::Escape) {
                        
                        for (auto it = windows.begin(); it != windows.end(); ++it) {
                            if (it->window->get_handle() == event_hwnd) {
                                std::println("   → ESC pressed, closing: {}", it->name);
                                it->window->close();
                                windows.erase(it);
                                break;
                            }
                        }
                    }
                }
            }

            // Check if all windows closed
            if (windows.empty() && Application::window_count() == 0) {
                std::println("\n✅ All windows closed");
                Application::shutdown();
                break;
            }

            // Small sleep to reduce CPU usage
            Sleep(1);
        }

        std::println("\n=== Application Exited Successfully ===");
        return 0;

    } catch (const std::exception& e) {
        std::println("❌ Exception: {}", e.what());
        Application::shutdown();
        return 1;
    }
}