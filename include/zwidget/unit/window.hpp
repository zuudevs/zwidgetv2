#pragma once

#include "zwidget/core/application.hpp"
#include "zwidget/core/event_dispatcher.hpp"
#include "zwidget/graphic/renderer.hpp"
#include "zwidget/unit/point.hpp"
#include "zwidget/unit/size.hpp"
#include "zwidget/unit/rect.hpp"
#include <string>
#include <atomic>
#include <functional>
#include <Windows.h>

namespace zuu::widget {

    enum class WindowStyle : DWORD {
        Default         = WS_OVERLAPPEDWINDOW,
        Overlapped      = WS_OVERLAPPEDWINDOW,
        Popup           = WS_POPUP,
        Child           = WS_CHILD,
        Minimized       = WS_MINIMIZE,
        Visible         = WS_VISIBLE,
        Disabled        = WS_DISABLED,
        Borderless      = WS_POPUP | WS_VISIBLE,
        FixedSize       = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    };

    class Window {
        friend class Application;
        friend LRESULT CALLBACK GlobalWindowProc(HWND, UINT, WPARAM, LPARAM);

    public:
        using PaintCallback = std::function<void(Renderer&)>;

    private:
        HWND hwnd_ = nullptr;
        std::atomic<uint32_t> state_{0};
        std::string title_;
        Renderer renderer_;
        PaintCallback paint_callback_;

        // HWND hanya bisa diakses oleh Application dan GlobalWindowProc
        HWND get_handle() const noexcept {
            return hwnd_;
        }

        void internal_destroy() noexcept {
            if (has_state_flag(WindowState::Destroyed)) {
                return;
            }

            if (has_state_flag(WindowState::Registered)) {
                Application::unregister_window(hwnd_);
                clear_state_flag(WindowState::Registered);
                set_state_flag(WindowState::Unregistered);
            }

            if (hwnd_ && IsWindow(hwnd_)) {
                DestroyWindow(hwnd_);
            }

            hwnd_ = nullptr;
            set_state_flag(WindowState::Destroyed);
        }

        // Thread-safe state operations
        void set_state_flag(WindowState flag) noexcept {
            uint32_t current = state_.load(std::memory_order_relaxed);
            uint32_t desired;
            do {
                desired = current | static_cast<uint32_t>(flag);
            } while (!state_.compare_exchange_weak(
                current, desired,
                std::memory_order_release,
                std::memory_order_relaxed
            ));
        }

        void clear_state_flag(WindowState flag) noexcept {
            uint32_t current = state_.load(std::memory_order_relaxed);
            uint32_t desired;
            do {
                desired = current & ~static_cast<uint32_t>(flag);
            } while (!state_.compare_exchange_weak(
                current, desired,
                std::memory_order_release,
                std::memory_order_relaxed
            ));
        }

        bool has_state_flag(WindowState flag) const noexcept {
            uint32_t current = state_.load(std::memory_order_acquire);
            return (current & static_cast<uint32_t>(flag)) != 0;
        }

        // Handle WM_PAINT
        void handle_paint() {
            if (!renderer_.is_initialized()) return;

            PAINTSTRUCT ps;
            BeginPaint(hwnd_, &ps);

            // Render dengan paint callback
            if (paint_callback_) {
                renderer_.render([this](Renderer& r) {
                    paint_callback_(r);
                });
            }

            EndPaint(hwnd_, &ps);
        }

    public:
        Window() = delete;
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(
            const std::string& title,
            const basic_size<int>& size,
            WindowStyle style = WindowStyle::Default
        ) : title_(title), state_(0) {
            if (!Application::is_class_registered()) {
                Application::initialize();
            }

            hwnd_ = CreateWindowExA(
                0,
                Application::get_window_class_name().c_str(),
                title_.c_str(),
                static_cast<DWORD>(style),
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                size.w,
                size.h,
                nullptr,
                nullptr,
                Application::get_hinstance(),
                nullptr
            );

            if (!hwnd_) {
                throw std::runtime_error("Failed to create window");
            }

            set_state_flag(WindowState::Active);
            Application::register_window(hwnd_, this);
            set_state_flag(WindowState::Registered);

            // Initialize renderer
            if (!renderer_.initialize(hwnd_, size)) {
                throw std::runtime_error("Failed to initialize renderer");
            }
        }

        Window(
            const std::string& title,
            const basic_point<int>& position,
            const basic_size<int>& size,
            WindowStyle style = WindowStyle::Default
        ) : title_(title), state_(0) {
            if (!Application::is_class_registered()) {
                Application::initialize();
            }

            hwnd_ = CreateWindowExA(
                0,
                Application::get_window_class_name().c_str(),
                title_.c_str(),
                static_cast<DWORD>(style),
                position.x,
                position.y,
                size.w,
                size.h,
                nullptr,
                nullptr,
                Application::get_hinstance(),
                nullptr
            );

            if (!hwnd_) {
                throw std::runtime_error("Failed to create window");
            }

            set_state_flag(WindowState::Active);
            Application::register_window(hwnd_, this);
            set_state_flag(WindowState::Registered);

            // Initialize renderer
            if (!renderer_.initialize(hwnd_, size)) {
                throw std::runtime_error("Failed to initialize renderer");
            }
        }

        Window(
            const std::string& title,
            const basic_rect<int>& rect,
            WindowStyle style = WindowStyle::Default
        ) : title_(title), state_(0) {
            if (!Application::is_class_registered()) {
                Application::initialize();
            }

            hwnd_ = CreateWindowExA(
                0,
                Application::get_window_class_name().c_str(),
                title_.c_str(),
                static_cast<DWORD>(style),
                rect.x,
                rect.y,
                rect.w,
                rect.h,
                nullptr,
                nullptr,
                Application::get_hinstance(),
                nullptr
            );

            if (!hwnd_) {
                throw std::runtime_error("Failed to create window");
            }

            set_state_flag(WindowState::Active);
            Application::register_window(hwnd_, this);
            set_state_flag(WindowState::Registered);

            // Initialize renderer
            if (!renderer_.initialize(hwnd_, basic_size<int>(rect.w, rect.h))) {
                throw std::runtime_error("Failed to initialize renderer");
            }
        }

        Window(Window&& other) noexcept
            : hwnd_(std::exchange(other.hwnd_, nullptr))
            , state_(other.state_.exchange(0, std::memory_order_acq_rel))
            , title_(std::move(other.title_))
            , renderer_(std::move(other.renderer_))
            , paint_callback_(std::move(other.paint_callback_)) {
            
            if (hwnd_) {
                Application::unregister_window(hwnd_);
                Application::register_window(hwnd_, this);
            }
        }

        Window& operator=(Window&& other) noexcept {
            if (this != &other) {
                internal_destroy();

                hwnd_ = std::exchange(other.hwnd_, nullptr);
                state_.store(other.state_.exchange(0, std::memory_order_acq_rel), 
                           std::memory_order_release);
                title_ = std::move(other.title_);
                renderer_ = std::move(other.renderer_);
                paint_callback_ = std::move(other.paint_callback_);

                if (hwnd_) {
                    Application::unregister_window(hwnd_);
                    Application::register_window(hwnd_, this);
                }
            }
            return *this;
        }

        ~Window() noexcept {
            internal_destroy();
        }

        // Window operations
        void show() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_SHOW);
                UpdateWindow(hwnd_);
                set_state_flag(WindowState::Visible);
            }
        }

        void hide() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_HIDE);
                clear_state_flag(WindowState::Visible);
            }
        }

        void minimize() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_MINIMIZE);
                set_state_flag(WindowState::Minimized);
                clear_state_flag(WindowState::Maximized);
            }
        }

        void maximize() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_MAXIMIZE);
                set_state_flag(WindowState::Maximized);
                clear_state_flag(WindowState::Minimized);
            }
        }

        void restore() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_RESTORE);
                clear_state_flag(WindowState::Minimized);
                clear_state_flag(WindowState::Maximized);
            }
        }

        void close() noexcept {
            if (hwnd_) {
                PostMessageA(hwnd_, WM_CLOSE, 0, 0);
            }
        }

        void focus() noexcept {
            if (hwnd_) {
                SetFocus(hwnd_);
            }
        }

        void set_title(const std::string& title) {
            title_ = title;
            if (hwnd_) {
                SetWindowTextA(hwnd_, title_.c_str());
            }
        }

        void set_position(const basic_point<int>& position) {
            if (hwnd_) {
                SetWindowPos(hwnd_, nullptr, position.x, position.y, 0, 0,
                           SWP_NOSIZE | SWP_NOZORDER);
            }
        }

        void set_size(const basic_size<int>& size) {
            if (hwnd_) {
                SetWindowPos(hwnd_, nullptr, 0, 0, size.w, size.h,
                           SWP_NOMOVE | SWP_NOZORDER);
            }
        }

        void set_bounds(const basic_rect<int>& rect) {
            if (hwnd_) {
                SetWindowPos(hwnd_, nullptr, rect.x, rect.y, rect.w, rect.h,
                           SWP_NOZORDER);
            }
        }

        // Rendering operations
        void set_paint_callback(PaintCallback callback) {
            paint_callback_ = std::move(callback);
        }

        void invalidate() {
            renderer_.invalidate_full();
        }

        void invalidate(const basic_rect<int>& region) {
            renderer_.invalidate(region);
        }

        Renderer& get_renderer() noexcept {
            return renderer_;
        }

        const Renderer& get_renderer() const noexcept {
            return renderer_;
        }

        // Property getters
        const std::string& get_title() const noexcept {
            return title_;
        }

        WindowState get_state() const noexcept {
            return static_cast<WindowState>(state_.load(std::memory_order_acquire));
        }

        basic_size<int> get_size() const noexcept {
            if (hwnd_) {
                RECT rect;
                GetClientRect(hwnd_, &rect);
                return basic_size<int>(rect.right - rect.left, rect.bottom - rect.top);
            }
            return basic_size<int>(0, 0);
        }

        basic_point<int> get_position() const noexcept {
            if (hwnd_) {
                RECT rect;
                GetWindowRect(hwnd_, &rect);
                return basic_point<int>(rect.left, rect.top);
            }
            return basic_point<int>(0, 0);
        }

        basic_rect<int> get_client_rect() const noexcept {
            if (hwnd_) {
                RECT rect;
                GetClientRect(hwnd_, &rect);
                return basic_rect<int>(0, 0, rect.right - rect.left, rect.bottom - rect.top);
            }
            return basic_rect<int>(0, 0, 0, 0);
        }

        basic_rect<int> get_window_rect() const noexcept {
            if (hwnd_) {
                RECT rect;
                GetWindowRect(hwnd_, &rect);
                return basic_rect<int>(
                    rect.left,
                    rect.top,
                    rect.right - rect.left,
                    rect.bottom - rect.top
                );
            }
            return basic_rect<int>(0, 0, 0, 0);
        }

        bool is_valid() const noexcept {
            return hwnd_ && !has_state_flag(WindowState::Destroyed);
        }

        bool is_visible() const noexcept {
            return hwnd_ && IsWindowVisible(hwnd_);
        }

        bool is_minimized() const noexcept {
            return hwnd_ && IsIconic(hwnd_);
        }

        bool is_maximized() const noexcept {
            return hwnd_ && IsZoomed(hwnd_);
        }

        bool has_focus() const noexcept {
            return hwnd_ && GetFocus() == hwnd_;
        }

        bool is_close_requested() const noexcept {
            return has_state_flag(WindowState::CloseRequested);
        }
    };

    inline LRESULT CALLBACK GlobalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window* window = Application::get_window(hwnd);

        switch (msg) {
            case WM_PAINT: {
                if (window) {
                    window->handle_paint();
                }
                return 0;
            }

            case WM_SIZE: {
                if (window && wParam != SIZE_MINIMIZED) {
                    auto new_size = basic_size<int>(LOWORD(lParam), HIWORD(lParam));
                    window->renderer_.resize(new_size);
                }

                WindowEvent::Type type;
                switch (wParam) {
                    case SIZE_MINIMIZED:
                        type = WindowEvent::Type::minimize;
                        if (window) window->set_state_flag(WindowState::Minimized);
                        break;
                    case SIZE_MAXIMIZED:
                        type = WindowEvent::Type::maximize;
                        if (window) window->set_state_flag(WindowState::Maximized);
                        break;
                    case SIZE_RESTORED:
                    default:
                        type = WindowEvent::Type::restored;
                        if (window) {
                            window->clear_state_flag(WindowState::Minimized);
                            window->clear_state_flag(WindowState::Maximized);
                        }
                        break;
                }

                Event event = Event::create_window_event(
                    window,
                    WindowEvent(
                        type,
                        basic_size<uint16_t>(
                            static_cast<uint16_t>(LOWORD(lParam)),
                            static_cast<uint16_t>(HIWORD(lParam))
                        )
                    )
                );
                EventDispatcher::push_event(event);
                break;
            }

            case WM_CLOSE: {
                if (window) {
                    window->set_state_flag(WindowState::CloseRequested);
                }

                Event event = Event::create_window_event(
                    window,
                    WindowEvent(WindowEvent::Type::close)
                );
                EventDispatcher::push_event(event);
                
                return 0;
            }

            case WM_DESTROY: {
                Application::unregister_window(hwnd);

                if (Application::window_count() == 0) {
                    Application::is_running_.store(false, std::memory_order_release);
                    PostQuitMessage(0);
                }
                return 0;
            }

            case WM_SETFOCUS: {
                if (window) {
                    window->set_state_flag(WindowState::Focused);
                }

                Event event = Event::create_window_event(
                    window,
                    WindowEvent(WindowEvent::Type::focus_gained)
                );
                EventDispatcher::push_event(event);
                break;
            }

            case WM_KILLFOCUS: {
                if (window) {
                    window->clear_state_flag(WindowState::Focused);
                }

                Event event = Event::create_window_event(
                    window,
                    WindowEvent(WindowEvent::Type::focus_lost)
                );
                EventDispatcher::push_event(event);
                break;
            }

            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            case WM_MOUSEWHEEL: {
                MSG temp_msg = { hwnd, msg, wParam, lParam };
                Event event = detail::CreateEventFromMSG(window, temp_msg);
                if (event.get_type() != Event::Type::none) {
                    EventDispatcher::push_event(event);
                }
                break;
            }

            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP: {
                MSG temp_msg = { hwnd, msg, wParam, lParam };
                Event event = detail::CreateEventFromMSG(window, temp_msg);
                if (event.get_type() != Event::Type::none) {
                    EventDispatcher::push_event(event);
                }
                break;
            }
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

} // namespace zuu::widget