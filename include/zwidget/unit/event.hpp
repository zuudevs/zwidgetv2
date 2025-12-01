#pragma once

#include "events/window.hpp"
#include "events/mouse.hpp"
#include "events/keyboard.hpp"
#include "zwidget/detail/variant.hpp"

#include <Windows.h>
#include <Windowsx.h>

#if defined(max) && defined(min)
    #undef max
    #undef min
#endif

namespace zuu::widget {
    class Event {
    public:
        enum class Type : uint8_t {
            none,
            window,
            mouse,
            keyboard,
            Quit,
        };

        class EmptyEvent {};

        using events_t = std::variant<EmptyEvent, WindowEvent, MouseEvent, KeyboardEvent>;

    private:
        HWND hwnd_{};
        Type type_ = Type::none;
        events_t event{};

        constexpr Event(HWND hwnd, Type type, events_t event) noexcept
            : hwnd_(hwnd), type_(type), event(event) {}

    public:
        constexpr Event() noexcept = default;
        constexpr Event(const Event&) noexcept = default;
        constexpr Event(Event&&) noexcept = default;
        constexpr Event& operator=(const Event&) noexcept = default;
        constexpr Event& operator=(Event&&) noexcept = default;
        constexpr ~Event() noexcept = default;

        static constexpr Event create_window_event(HWND hwnd, const WindowEvent& event) noexcept {
            auto type = event.get_type();
            bool has_size = !event.get_size().is_empty();

            if (type < WindowEvent::Type::resize && has_size) {
                return Event{};
            }

            if (type >= WindowEvent::Type::resize && !has_size) {
                return Event{};
            }

            return Event(hwnd, Type::window, event);
        }

        static constexpr Event create_mouse_event(HWND hwnd, const MouseEvent& event) noexcept {
            auto type = event.get_type();
            bool has_position = !event.get_position().is_empty();

            if (type == MouseEvent::Type::scroll && has_position) {
                return Event{};
            }

            if (type > MouseEvent::Type::scroll && !has_position) {
                return Event{};
            }

            return Event(hwnd, Type::mouse, event);
        }

        static constexpr Event create_keyboard_event(HWND hwnd, const KeyboardEvent& event) noexcept {
            return Event(hwnd, Type::keyboard, event);
        }

        static constexpr Event create_quit_event() noexcept {
            return Event(nullptr, Type::Quit, EmptyEvent{});
        }

        // Getters
        constexpr HWND get_hwnd() const noexcept {
            return hwnd_;
        }

        constexpr Type get_type() const noexcept {
            return type_;
        }

        template <meta::variant_type<events_t> T>
        [[nodiscard]] constexpr const T* get_if() const noexcept {
            return std::get_if<T>(&event);
        }

        template <meta::variant_type<events_t> T>
        [[nodiscard]] constexpr T* get_if() noexcept {
            return std::get_if<T>(&event);
        }

        template <meta::variant_type<events_t> T>
        constexpr bool is_type() const noexcept {
            return std::holds_alternative<T>(event);
        }

        // Helper methods untuk quick check
        constexpr bool is_window_event() const noexcept {
            return type_ == Type::window;
        }

        constexpr bool is_mouse_event() const noexcept {
            return type_ == Type::mouse;
        }

        constexpr bool is_keyboard_event() const noexcept {
            return type_ == Type::keyboard;
        }

        constexpr bool is_quit_event() const noexcept {
            return type_ == Type::Quit;
        }
    };
} // namespace zuu::widget