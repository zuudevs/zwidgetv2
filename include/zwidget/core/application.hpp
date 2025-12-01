#pragma once

#include <Windows.h>
#include <unordered_map>
#include <vector>
#include <string>

#if defined(max) && defined(min)
    #undef max
    #undef min
#endif

namespace zuu::widget {

    // Forward declarations
    class Window;
    
    // Window state flags
    enum class WindowState : uint32_t {
        None            = 0,
        Active          = 1 << 0,
        Visible         = 1 << 1,
        Minimized       = 1 << 2,
        Maximized       = 1 << 3,
        Focused         = 1 << 4,
        Registered      = 1 << 5,
        Unregistered    = 1 << 6,
        Destroyed       = 1 << 7,
        CloseRequested  = 1 << 8,
    };

    // Bitwise operators for WindowState
    constexpr WindowState operator|(WindowState lhs, WindowState rhs) noexcept {
        return static_cast<WindowState>(
            static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
        );
    }

    constexpr WindowState operator&(WindowState lhs, WindowState rhs) noexcept {
        return static_cast<WindowState>(
            static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
        );
    }

    constexpr WindowState operator~(WindowState state) noexcept {
        return static_cast<WindowState>(~static_cast<uint32_t>(state));
    }

    constexpr WindowState& operator|=(WindowState& lhs, WindowState rhs) noexcept {
        lhs = lhs | rhs;
        return lhs;
    }

    constexpr WindowState& operator&=(WindowState& lhs, WindowState rhs) noexcept {
        lhs = lhs & rhs;
        return lhs;
    }

    constexpr bool has_state(WindowState state, WindowState check) noexcept {
        return (state & check) == check;
    }

    LRESULT CALLBACK GlobalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class Application {
        friend class Window;
        friend LRESULT CALLBACK GlobalWindowProc(HWND, UINT, WPARAM, LPARAM);

    private:
        // Window registry: HWND -> Window*
        static inline std::unordered_map<HWND, Window*> window_registry_{};
        
        // Application state
        static inline bool is_running_ = true;
        static inline HINSTANCE hinstance_ = GetModuleHandleW(nullptr);
        static inline std::string window_class_name_ = "ZWidgetWindowClass";
        static inline bool class_registered_ = false;

        // Register window to registry
        static void register_window(HWND hwnd, Window* window) noexcept {
            if (hwnd && window) {
                window_registry_[hwnd] = window;
            }
        }

        // Unregister window from registry
        static void unregister_window(HWND hwnd) noexcept {
            if (hwnd) {
                auto it = window_registry_.find(hwnd);
                if (it != window_registry_.end()) {
                    window_registry_.erase(it);
                }
            }
        }

        // Get window from registry
        static Window* get_window(HWND hwnd) noexcept {
            auto it = window_registry_.find(hwnd);
            return (it != window_registry_.end()) ? it->second : nullptr;
        }

    public:
        Application() = delete;
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        static bool initialize(const std::string& window_class_name = "ZWidgetWindowClass") {
            if (class_registered_) {
                return true; // Already registered
            }

            window_class_name_ = window_class_name;

            WNDCLASSEXA wc = {};
            wc.cbSize = sizeof(WNDCLASSEXA);
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wc.lpfnWndProc = GlobalWindowProc;
            wc.hInstance = hinstance_;
            wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wc.lpszClassName = window_class_name_.c_str();
            wc.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
            wc.hIconSm = LoadIconA(nullptr, IDI_APPLICATION);

            if (!RegisterClassExA(&wc)) {
                return false;
            }

            class_registered_ = true;
            is_running_ = true;
            return true;
        }

        static void shutdown() noexcept {
            // Collect all window handles
            std::vector<HWND> handles;
            handles.reserve(window_registry_.size());
            for (const auto& [hwnd, _] : window_registry_) {
                handles.push_back(hwnd);
            }

            // Destroy all windows
            for (HWND hwnd : handles) {
                if (IsWindow(hwnd)) {
                    DestroyWindow(hwnd);
                }
            }

            window_registry_.clear();
            is_running_ = false;
            PostQuitMessage(0);
        }

        static bool is_running() noexcept {
            return is_running_;
        }

        static size_t window_count() noexcept {
            return window_registry_.size();
        }

        static HINSTANCE get_hinstance() noexcept {
            return hinstance_;
        }

        static const std::string& get_window_class_name() noexcept {
            return window_class_name_;
        }

        static bool is_class_registered() noexcept {
            return class_registered_;
        }
    };

} // namespace zuu::widget