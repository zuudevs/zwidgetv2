#pragma once

#include <Windows.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <shared_mutex>
#include <atomic>

#if defined(max) && defined(min)
    #undef max
    #undef min
#endif

namespace zuu::widget {

    class Window;
	class EventDispatcher;
    
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
		friend class EventDispatcher;
        friend LRESULT CALLBACK GlobalWindowProc(HWND, UINT, WPARAM, LPARAM);

    private:
        static inline std::unordered_map<HWND, Window*> window_registry_{};
        static inline std::shared_mutex registry_mutex_{};
        
        static inline std::atomic<bool> is_running_{true};
        static inline HINSTANCE hinstance_;
        static inline std::string window_class_name_;
        static inline std::atomic<bool> class_registered_{false};

        static void register_window(HWND hwnd, Window* window) noexcept {
            if (hwnd && window) {
                std::unique_lock<std::shared_mutex> lock(registry_mutex_);
                window_registry_[hwnd] = window;
            }
        }

        static void unregister_window(HWND hwnd) noexcept {
            if (hwnd) {
                std::unique_lock<std::shared_mutex> lock(registry_mutex_);
                auto it = window_registry_.find(hwnd);
                if (it != window_registry_.end()) {
                    window_registry_.erase(it);
                }
            }
        }

        static Window* get_window(HWND hwnd) noexcept {
			std::shared_lock<std::shared_mutex> lock(registry_mutex_);
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
            // Double-checked locking pattern
            if (class_registered_.load(std::memory_order_acquire)) {
                return true;
            }

            // Static initialization
            static bool initialized = false;
            if (!initialized) {
                hinstance_ = GetModuleHandleW(nullptr);
                window_class_name_ = window_class_name;
                initialized = true;
            }

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

            class_registered_.store(true, std::memory_order_release);
            is_running_.store(true, std::memory_order_release);
            return true;
        }

        static void shutdown() noexcept {
            std::vector<HWND> handles;
            {
                std::shared_lock<std::shared_mutex> lock(registry_mutex_);
                handles.reserve(window_registry_.size());
                for (const auto& [hwnd, _] : window_registry_) {
                    handles.push_back(hwnd);
                }
            }

            for (HWND hwnd : handles) {
                if (IsWindow(hwnd)) {
                    DestroyWindow(hwnd);
                }
            }

            {
                std::unique_lock<std::shared_mutex> lock(registry_mutex_);
                window_registry_.clear();
            }
            
            is_running_.store(false, std::memory_order_release);
            PostQuitMessage(0);
        }

        static bool is_running() noexcept {
            return is_running_.load(std::memory_order_acquire);
        }

        static size_t window_count() noexcept {
            std::shared_lock<std::shared_mutex> lock(registry_mutex_);
            return window_registry_.size();
        }

        static HINSTANCE get_hinstance() noexcept {
            return hinstance_;
        }

        static const std::string& get_window_class_name() noexcept {
            return window_class_name_;
        }

        static bool is_class_registered() noexcept {
            return class_registered_.load(std::memory_order_acquire);
        }
    };

} // namespace zuu::widget