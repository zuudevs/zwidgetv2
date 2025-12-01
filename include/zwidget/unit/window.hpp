#pragma once

#include "zwidget/core/application.hpp"
#include "zwidget/core/event_dispatcher.hpp"
#include "zwidget/unit/point.hpp"
#include "zwidget/unit/size.hpp"
#include "zwidget/unit/rect.hpp"
#include <string>
#include <Windows.h>

namespace zuu::widget {

    // Window styles enum for convenience
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

    private:
        HWND hwnd_ = nullptr;
        WindowState state_ = WindowState::None;
        std::string title_;

        void internal_destroy() noexcept {
            if (has_state(state_, WindowState::Destroyed)) {
                return;
            }

            if (has_state(state_, WindowState::Registered)) {
                Application::unregister_window(hwnd_);
                state_ &= ~WindowState::Registered;
                state_ |= WindowState::Unregistered;
            }

            if (hwnd_ && IsWindow(hwnd_)) {
                DestroyWindow(hwnd_);
            }

            hwnd_ = nullptr;
            state_ |= WindowState::Destroyed;
        }

    public:
        Window() = delete;
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(
            const std::string& title,
            const basic_size<int>& size,
            WindowStyle style = WindowStyle::Default
        ) : title_(title) {
            // Ensure Application is initialized
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

            state_ = WindowState::Active;
            Application::register_window(hwnd_, this);
            state_ |= WindowState::Registered;
        }

        Window(
            const std::string& title,
            const basic_point<int>& position,
            const basic_size<int>& size,
            WindowStyle style = WindowStyle::Default
        ) : title_(title) {
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

            state_ = WindowState::Active;
            Application::register_window(hwnd_, this);
            state_ |= WindowState::Registered;
        }

        Window(
            const std::string& title,
            const basic_rect<int>& rect,
            WindowStyle style = WindowStyle::Default
        ) : title_(title) {
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

            state_ = WindowState::Active;
            Application::register_window(hwnd_, this);
            state_ |= WindowState::Registered;
        }

        Window(Window&& other) noexcept
            : hwnd_(std::exchange(other.hwnd_, nullptr))
            , state_(std::exchange(other.state_, WindowState::None))
            , title_(std::move(other.title_)) {
            
            if (hwnd_) {
                Application::unregister_window(hwnd_);
                Application::register_window(hwnd_, this);
            }
        }

        Window& operator=(Window&& other) noexcept {
            if (this != &other) {
                internal_destroy();

                hwnd_ = std::exchange(other.hwnd_, nullptr);
                state_ = std::exchange(other.state_, WindowState::None);
                title_ = std::move(other.title_);

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

        void show() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_SHOW);
                UpdateWindow(hwnd_);
                state_ |= WindowState::Visible;
            }
        }

        void hide() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_HIDE);
                state_ &= ~WindowState::Visible;
            }
        }

        void minimize() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_MINIMIZE);
                state_ |= WindowState::Minimized;
                state_ &= ~WindowState::Maximized;
            }
        }

        void maximize() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_MAXIMIZE);
                state_ |= WindowState::Maximized;
                state_ &= ~WindowState::Minimized;
            }
        }

        void restore() noexcept {
            if (hwnd_) {
                ShowWindow(hwnd_, SW_RESTORE);
                state_ &= ~WindowState::Minimized;
                state_ &= ~WindowState::Maximized;
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

        HWND get_handle() const noexcept {
            return hwnd_;
        }

        const std::string& get_title() const noexcept {
            return title_;
        }

        WindowState get_state() const noexcept {
            return state_;
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
            return hwnd_ && !has_state(state_, WindowState::Destroyed);
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
            return has_state(state_, WindowState::CloseRequested);
        }
    };

    inline LRESULT CALLBACK GlobalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window* window = Application::get_window(hwnd);

        switch (msg) {
            case WM_CLOSE: {
                if (window) {
                    window->state_ |= WindowState::CloseRequested;
                }

                Event event = Event::create_window_event(
                    hwnd,
                    WindowEvent(WindowEvent::Type::close)
                );
                EventDispatcher::push_event(event);
                
                return 0;
            }

            case WM_DESTROY: {
                Application::unregister_window(hwnd);

                // If no more windows, quit application
                if (Application::window_count() == 0) {
                    Application::is_running_ = false;
                    PostQuitMessage(0);
                }
                return 0;
            }

            case WM_SIZE: {
                WindowEvent::Type type;
                switch (wParam) {
                    case SIZE_MINIMIZED:
                        type = WindowEvent::Type::minimize;
                        if (window) window->state_ |= WindowState::Minimized;
                        break;
                    case SIZE_MAXIMIZED:
                        type = WindowEvent::Type::maximize;
                        if (window) window->state_ |= WindowState::Maximized;
                        break;
                    case SIZE_RESTORED:
                    default:
                        type = WindowEvent::Type::restored;
                        if (window) {
                            window->state_ &= ~WindowState::Minimized;
                            window->state_ &= ~WindowState::Maximized;
                        }
                        break;
                }

                Event event = Event::create_window_event(
                    hwnd,
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

            case WM_SETFOCUS: {
                if (window) {
                    window->state_ |= WindowState::Focused;
                }

                Event event = Event::create_window_event(
                    hwnd,
                    WindowEvent(WindowEvent::Type::focus_gained)
                );
                EventDispatcher::push_event(event);
                break;
            }

            case WM_KILLFOCUS: {
                if (window) {
                    window->state_ &= ~WindowState::Focused;
                }

                Event event = Event::create_window_event(
                    hwnd,
                    WindowEvent(WindowEvent::Type::focus_lost)
                );
                EventDispatcher::push_event(event);
                break;
            }

            // Mouse events
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
                Event event = detail::CreateEventFromMSG(temp_msg);
                if (event.get_type() != Event::Type::none) {
                    EventDispatcher::push_event(event);
                }
                break;
            }

            // Keyboard events
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP: {
                MSG temp_msg = { hwnd, msg, wParam, lParam };
                Event event = detail::CreateEventFromMSG(temp_msg);
                if (event.get_type() != Event::Type::none) {
                    EventDispatcher::push_event(event);
                }
                break;
            }
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

} // namespace zuu::widget