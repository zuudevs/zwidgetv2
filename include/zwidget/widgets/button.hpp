#pragma once

#include "widget.hpp"

namespace zuu::widget {

    class Button : public Widget {
    private:
        std::wstring text_;
        std::function<void(Button*)> on_click_;
        
        Color normal_bg_{Color::from_hex(0x4a4a4a)};
        Color hover_bg_{Color::from_hex(0x5a5a5a)};
        Color pressed_bg_{Color::from_hex(0x3a3a3a)};
        Color disabled_bg_{Color::from_hex(0x2a2a2a)};

    public:
        Button() {
            set_focusable(true);
            style_.border_radius = 4.0f;
            style_.padding = {10.0f, 5.0f, 10.0f, 5.0f};
        }

        explicit Button(const std::wstring& text) : Button() {
            text_ = text;
        }

        void render(Renderer& renderer) override {
            if (!is_visible()) return;

            // Determine background color based on state
            Color bg_color = normal_bg_;
            if (!is_enabled()) {
                bg_color = disabled_bg_;
            } else if (is_pressed()) {
                bg_color = pressed_bg_;
            } else if (is_hovered()) {
                bg_color = hover_bg_;
            }

            // Draw background
            if (style_.border_radius > 0) {
                renderer.fill_rounded_rect(
                    bounds_,
                    style_.border_radius,
                    style_.border_radius,
                    bg_color
                );
            } else {
                renderer.fill_rect(bounds_, bg_color);
            }

            // Draw border
            if (style_.border_width > 0) {
                Color border = style_.border_color;
                if (is_focused()) {
                    border = Color::from_hex(0x4a90e2);
                }

                if (style_.border_radius > 0) {
                    renderer.draw_rounded_rect(
                        bounds_,
                        style_.border_radius,
                        style_.border_radius,
                        border,
                        style_.border_width
                    );
                } else {
                    renderer.draw_rect(bounds_, border, style_.border_width);
                }
            }

            // Draw text
            if (!text_.empty()) {
                Color text_color = style_.text_color;
                if (!is_enabled()) {
                    text_color = Color(0.5f, 0.5f, 0.5f, 0.5f);
                }

                renderer.draw_text(text_, content_bounds_, text_color);
            }

            set_flag(WidgetFlag::Dirty, false);
        }

        bool handle_mouse_down(const MouseEvent& event) override {
            if (!is_enabled()) return false;

            if (event.get_button() == MouseEvent::Button::left) {
                set_pressed(true);
                return true;
            }

            return Widget::handle_mouse_down(event);
        }

        bool handle_mouse_up(const MouseEvent& event) override {
            if (!is_enabled()) return false;

            if (event.get_button() == MouseEvent::Button::left) {
                bool was_pressed = is_pressed();
                set_pressed(false);

                // Trigger click if mouse is still over button
                if (was_pressed && is_hovered() && on_click_) {
                    on_click_(this);
                }

                return true;
            }

            return Widget::handle_mouse_up(event);
        }

        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_enabled()) return false;

            // Activate on Space or Enter
            if (event.get_key() == KeyboardEvent::KeyCode::Space ||
                event.get_key() == KeyboardEvent::KeyCode::Enter) {
                set_pressed(true);
                if (on_click_) {
                    on_click_(this);
                }
                return true;
            }

            return Widget::handle_key_down(event);
        }

        bool handle_key_up(const KeyboardEvent& event) override {
            if (!is_enabled()) return false;

            if (event.get_key() == KeyboardEvent::KeyCode::Space ||
                event.get_key() == KeyboardEvent::KeyCode::Enter) {
                set_pressed(false);
                return true;
            }

            return Widget::handle_key_up(event);
        }

        // Setters
        void set_text(const std::wstring& text) {
            if (text_ != text) {
                text_ = text;
                mark_dirty();
            }
        }

        void on_click(std::function<void(Button*)> callback) {
            on_click_ = std::move(callback);
        }

        void set_colors(const Color& normal, const Color& hover, 
                       const Color& pressed, const Color& disabled) {
            normal_bg_ = normal;
            hover_bg_ = hover;
            pressed_bg_ = pressed;
            disabled_bg_ = disabled;
            mark_dirty();
        }

        // Getters
        const std::wstring& get_text() const noexcept {
            return text_;
        }
    };

} // namespace zuu::widget