#pragma once

#include "widget.hpp"

namespace zuu::widget {

    class TextBox : public Widget {
    private:
        std::wstring text_;
        std::wstring placeholder_;
        size_t cursor_position_{0};
        size_t selection_start_{0};
        size_t selection_end_{0};
        
        bool is_password_{false};
        wchar_t password_char_{L'•'};
        size_t max_length_{1024};
        
        float cursor_blink_time_{0.0f};
        bool cursor_visible_{true};
        
        Color background_normal_{Color::from_hex(0x3a3a3a)};
        Color background_focused_{Color::from_hex(0x454545)};
        Color selection_color_{Color::from_hex(0x4a90e2)};
        Color cursor_color_{Color::White()};
        
        std::function<void(TextBox*, const std::wstring&)> on_text_changed_;
        std::function<void(TextBox*)> on_enter_pressed_;

        void clamp_cursor() {
            cursor_position_ = std::min(cursor_position_, text_.length());
        }

        void delete_selection() {
            if (!has_selection()) return;
            
            size_t start = std::min(selection_start_, selection_end_);
            size_t end = std::max(selection_start_, selection_end_);
            
            text_.erase(start, end - start);
            cursor_position_ = start;
            selection_start_ = selection_end_ = 0;
            
            if (on_text_changed_) {
                on_text_changed_(this, text_);
            }
        }

        bool has_selection() const {
            return selection_start_ != selection_end_;
        }

        std::wstring get_display_text() const {
            if (is_password_ && !text_.empty()) {
                return std::wstring(text_.length(), password_char_);
            }
            return text_;
        }

    public:
        TextBox() {
            set_focusable(true);
            style_.border_width = 1.0f;
            style_.border_radius = 3.0f;
            style_.padding = {8.0f, 5.0f, 8.0f, 5.0f};
            style_.text_color = Color::White();
            style_.border_color = Color::Gray();
        }

        void render(Renderer& renderer) override {
            if (!is_visible()) return;

            // Determine background color
            Color bg = is_focused() ? background_focused_ : background_normal_;
            
            // Draw background
            if (style_.border_radius > 0) {
                renderer.fill_rounded_rect(
                    bounds_,
                    style_.border_radius,
                    style_.border_radius,
                    bg
                );
            } else {
                renderer.fill_rect(bounds_, bg);
            }

            // Draw border
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
                    style_.border_width * 2.0f
                );
            } else {
                renderer.draw_rect(bounds_, border, style_.border_width * 2.0f);
            }

            // Clip to content area
            renderer.push_clip(content_bounds_);

            // Draw selection
            if (has_selection() && is_focused()) {
                size_t start = std::min(selection_start_, selection_end_);
                size_t end = std::max(selection_start_, selection_end_);
                
                // Simplified selection rendering (full height)
                float sel_x = content_bounds_.x + start * 8.0f; // Rough estimate
                float sel_w = (end - start) * 8.0f;
                
                renderer.fill_rect(
                    basic_rect<float>(sel_x, content_bounds_.y, sel_w, content_bounds_.h),
                    selection_color_
                );
            }

            // Draw text or placeholder
            std::wstring display_text = get_display_text();
            
            if (display_text.empty() && !placeholder_.empty() && !is_focused()) {
                renderer.draw_text(
                    placeholder_,
                    content_bounds_,
                    Color(0.5f, 0.5f, 0.5f, 0.7f)
                );
            } else if (!display_text.empty()) {
                renderer.draw_text(display_text, content_bounds_, style_.text_color);
            }

            // Draw cursor
            if (is_focused() && cursor_visible_ && !has_selection()) {
                float cursor_x = content_bounds_.x + cursor_position_ * 8.0f; // Rough
                
                renderer.draw_line(
                    basic_point<float>(cursor_x, content_bounds_.y + 2),
                    basic_point<float>(cursor_x, content_bounds_.y + content_bounds_.h - 2),
                    cursor_color_,
                    2.0f
                );
            }

            renderer.pop_clip();

            set_flag(WidgetFlag::Dirty, false);
        }

        void update(float dt) override {
            Widget::update(dt);
            
            if (is_focused()) {
                cursor_blink_time_ += dt;
                if (cursor_blink_time_ >= 0.5f) {
                    cursor_visible_ = !cursor_visible_;
                    cursor_blink_time_ = 0.0f;
                    mark_dirty();
                }
            }
        }

        bool handle_mouse_down(const MouseEvent& event) override {
            if (!is_enabled()) return false;

            if (event.get_button() == MouseEvent::Button::left) {
                // TODO: Calculate actual cursor position from mouse click
                // For now, just set to end
                cursor_position_ = text_.length();
                selection_start_ = selection_end_ = 0;
                mark_dirty();
                return true;
            }

            return Widget::handle_mouse_down(event);
        }

        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_enabled() || !is_focused()) return false;

            bool handled = false;
            auto key = event.get_key();

            // Handle control keys
            if (key == KeyboardEvent::KeyCode::Left) {
                if (cursor_position_ > 0) {
                    cursor_position_--;
                    clamp_cursor();
                    cursor_blink_time_ = 0.0f;
                    cursor_visible_ = true;
                    handled = true;
                }
            }
            else if (key == KeyboardEvent::KeyCode::Right) {
                if (cursor_position_ < text_.length()) {
                    cursor_position_++;
                    clamp_cursor();
                    cursor_blink_time_ = 0.0f;
                    cursor_visible_ = true;
                    handled = true;
                }
            }
            else if (key == KeyboardEvent::KeyCode::Home) {
                cursor_position_ = 0;
                cursor_blink_time_ = 0.0f;
                cursor_visible_ = true;
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::End) {
                cursor_position_ = text_.length();
                cursor_blink_time_ = 0.0f;
                cursor_visible_ = true;
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::Back) {
                if (has_selection()) {
                    delete_selection();
                } else if (cursor_position_ > 0) {
                    text_.erase(cursor_position_ - 1, 1);
                    cursor_position_--;
                    if (on_text_changed_) {
                        on_text_changed_(this, text_);
                    }
                }
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::Delete) {
                if (has_selection()) {
                    delete_selection();
                } else if (cursor_position_ < text_.length()) {
                    text_.erase(cursor_position_, 1);
                    if (on_text_changed_) {
                        on_text_changed_(this, text_);
                    }
                }
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::Enter) {
                if (on_enter_pressed_) {
                    on_enter_pressed_(this);
                }
                handled = true;
            }
            else if (key >= KeyboardEvent::KeyCode::Space && 
                     key <= KeyboardEvent::KeyCode::Z) {
                // Character input
                if (text_.length() < max_length_) {
                    if (has_selection()) {
                        delete_selection();
                    }
                    
                    wchar_t ch = static_cast<wchar_t>(key);
                    text_.insert(cursor_position_, 1, ch);
                    cursor_position_++;
                    
                    if (on_text_changed_) {
                        on_text_changed_(this, text_);
                    }
                }
                handled = true;
            }

            if (handled) {
                mark_dirty();
            }

            return handled || Widget::handle_key_down(event);
        }

        // Setters
        void set_text(const std::wstring& text) {
            if (text_ != text) {
                text_ = text;
                cursor_position_ = text_.length();
                clamp_cursor();
                selection_start_ = selection_end_ = 0;
                mark_dirty();
                
                if (on_text_changed_) {
                    on_text_changed_(this, text_);
                }
            }
        }

        void set_placeholder(const std::wstring& placeholder) {
            if (placeholder_ != placeholder) {
                placeholder_ = placeholder;
                mark_dirty();
            }
        }

        void set_password_mode(bool enabled) {
            if (is_password_ != enabled) {
                is_password_ = enabled;
                mark_dirty();
            }
        }

        void set_max_length(size_t max_length) {
            max_length_ = max_length;
            if (text_.length() > max_length_) {
                text_.resize(max_length_);
                clamp_cursor();
                mark_dirty();
            }
        }

        void on_text_changed(std::function<void(TextBox*, const std::wstring&)> callback) {
            on_text_changed_ = std::move(callback);
        }

        void on_enter_pressed(std::function<void(TextBox*)> callback) {
            on_enter_pressed_ = std::move(callback);
        }

        // Getters
        const std::wstring& get_text() const noexcept {
            return text_;
        }

        bool is_password_mode() const noexcept {
            return is_password_;
        }

        size_t get_max_length() const noexcept {
            return max_length_;
        }
    };

} // namespace zuu::widget