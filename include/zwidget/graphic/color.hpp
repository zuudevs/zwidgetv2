#pragma once

#include <cstdint>
#include <d2d1.h>

namespace zuu::widget {

    class Color {
    private:
        float r_{0.0f};
        float g_{0.0f};
        float b_{0.0f};
        float a_{1.0f};

    public:
        constexpr Color() noexcept = default;
        
        constexpr Color(float r, float g, float b, float a = 1.0f) noexcept
            : r_(r), g_(g), b_(b), a_(a) {}

        static constexpr Color from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept {
            return Color(
                r / 255.0f,
                g / 255.0f,
                b / 255.0f,
                a / 255.0f
            );
        }

        static constexpr Color from_hex(uint32_t hex) noexcept {
            if (hex > 0xFFFFFF) {
                return Color(
                    ((hex >> 16) & 0xFF) / 255.0f,
                    ((hex >> 8) & 0xFF) / 255.0f,
                    (hex & 0xFF) / 255.0f,
                    ((hex >> 24) & 0xFF) / 255.0f
                );
            }
            return Color(
                ((hex >> 16) & 0xFF) / 255.0f,
                ((hex >> 8) & 0xFF) / 255.0f,
                (hex & 0xFF) / 255.0f,
                1.0f
            );
        }

        constexpr D2D1_COLOR_F to_d2d() const noexcept {
            return D2D1::ColorF(r_, g_, b_, a_);
        }

        constexpr float r() const noexcept { return r_; }
        constexpr float g() const noexcept { return g_; }
        constexpr float b() const noexcept { return b_; }
        constexpr float a() const noexcept { return a_; }

        constexpr void set_r(float r) noexcept { r_ = r; }
        constexpr void set_g(float g) noexcept { g_ = g; }
        constexpr void set_b(float b) noexcept { b_ = b; }
        constexpr void set_a(float a) noexcept { a_ = a; }

        static consteval Color White() noexcept { return Color(1.0f, 1.0f, 1.0f); }
        static consteval Color Black() noexcept { return Color(0.0f, 0.0f, 0.0f); }
        static consteval Color Red() noexcept { return Color(1.0f, 0.0f, 0.0f); }
        static consteval Color Green() noexcept { return Color(0.0f, 1.0f, 0.0f); }
        static consteval Color Blue() noexcept { return Color(0.0f, 0.0f, 1.0f); }
        static consteval Color Yellow() noexcept { return Color(1.0f, 1.0f, 0.0f); }
        static consteval Color Cyan() noexcept { return Color(0.0f, 1.0f, 1.0f); }
        static consteval Color Magenta() noexcept { return Color(1.0f, 0.0f, 1.0f); }
        static consteval Color Transparent() noexcept { return Color(0.0f, 0.0f, 0.0f, 0.0f); }
        static consteval Color Gray() noexcept { return Color(0.5f, 0.5f, 0.5f); }
        static consteval Color LightGray() noexcept { return Color(0.75f, 0.75f, 0.75f); }
        static consteval Color DarkGray() noexcept { return Color(0.25f, 0.25f, 0.25f); }
    };

} // namespace zuu::widget