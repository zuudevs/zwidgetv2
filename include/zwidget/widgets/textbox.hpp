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
        bool read_only_{false};
        
        float cursor_blink_time_{0.0f};
        bool cursor_visible_{true};
        float scroll_offset_{0.0f};  // For horizontal scrolling
        
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
            clear_selection();
            
            if (on_text_changed_) {
                on_text_changed_(this, text_);
            }
        }

        bool has_selection() const {
            return selection_start_ != selection_end_;
        }
        
        void clear_selection() {
            selection_start_ = selection_end_ = 0;
        }
        
        void select_all() {
            selection_start_ = 0;
            selection_end_ = text_.length();
            cursor_position_ = text_.length();
        }

        std::wstring get_display_text() const {
            if (is_password_ && !text_.empty()) {
                return std::wstring(text_.length(), password_char_);
            }
            return text_;
        }
        
        // Improved character input handling
        void insert_character(wchar_t ch) {
            if (read_only_ || text_.length() >= max_length_) return;
            
            if (has_selection()) {
                delete_selection();
            }
            
            text_.insert(cursor_position_, 1, ch);
            cursor_position_++;
            
            if (on_text_changed_) {
                on_text_changed_(this, text_);
            }
        }
        
        // Convert VK code to character with shift state
        wchar_t vk_to_char(KeyboardEvent::KeyCode key, bool shift) {
            // Get the virtual key code
            UINT vk = static_cast<UINT>(key);
            
            // Get keyboard state
            BYTE keyboard_state[256] = {0};
            if (shift) {
                keyboard_state[VK_SHIFT] = 0x80;
            }
            
            // Translate to character
            wchar_t buffer[2] = {0};
            int result = ToUnicode(vk, 0, keyboard_state, buffer, 2, 0);
            
            if (result == 1) {
                return buffer[0];
            }
            
            return 0;
        }
        
        bool is_shift_pressed() const {
            return (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        }
        
        bool is_ctrl_pressed() const {
            return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
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

            // Background
            Color bg = is_focused() ? background_focused_ : background_normal_;
            
            if (style_.border_radius > 0) {
                renderer.fill_rounded_rect(bounds_, style_.border_radius, style_.border_radius, bg);
            } else {
                renderer.fill_rect(bounds_, bg);
            }

            // Border
            Color border = is_focused() ? Color::from_hex(0x4a90e2) : style_.border_color;
            
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
                
                // Rough character width estimation (should use proper text measurement)
                float char_width = 8.0f;
                float sel_x = content_bounds_.x + start * char_width - scroll_offset_;
                float sel_w = (end - start) * char_width;
                
                renderer.fill_rect(
                    basic_rect<float>(sel_x, content_bounds_.y, sel_w, content_bounds_.h),
                    Color(selection_color_.r(), selection_color_.g(), selection_color_.b(), 0.3f)
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
                // Apply scroll offset for horizontal scrolling
                auto text_rect = content_bounds_;
                text_rect.x -= scroll_offset_;
                renderer.draw_text(display_text, text_rect, style_.text_color);
            }

            // Draw cursor
            if (is_focused() && cursor_visible_ && !has_selection()) {
                float char_width = 8.0f;
                float cursor_x = content_bounds_.x + cursor_position_ * char_width - scroll_offset_;
                
                // Auto-scroll to keep cursor visible
                if (cursor_x < content_bounds_.x) {
                    scroll_offset_ -= (content_bounds_.x - cursor_x);
                } else if (cursor_x > content_bounds_.x + content_bounds_.w) {
                    scroll_offset_ += (cursor_x - (content_bounds_.x + content_bounds_.w));
                }
                
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
                // TODO: Calculate actual cursor position from click
                // For now, simple implementation
                float char_width = 8.0f;
                float click_x = static_cast<float>(event.get_position().x) - content_bounds_.x + scroll_offset_;
                cursor_position_ = static_cast<size_t>(std::max(0.0f, click_x / char_width));
                clamp_cursor();
                
                if (!is_shift_pressed()) {
                    clear_selection();
                }
                
                mark_dirty();
                return true;
            }

            return Widget::handle_mouse_down(event);
        }

        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_enabled() || !is_focused()) return false;

            bool handled = false;
            auto key = event.get_key();
            bool shift = is_shift_pressed();
            bool ctrl = is_ctrl_pressed();

            // Handle Ctrl shortcuts
            if (ctrl) {
                if (key == KeyboardEvent::KeyCode::A) {
                    select_all();
                    handled = true;
                }
                else if (key == KeyboardEvent::KeyCode::C) {
                    // TODO: Copy to clipboard
                    handled = true;
                }
                else if (key == KeyboardEvent::KeyCode::V) {
                    // TODO: Paste from clipboard
                    handled = true;
                }
                else if (key == KeyboardEvent::KeyCode::X) {
                    // TODO: Cut to clipboard
                    handled = true;
                }
            }
            // Navigation keys
            else if (key == KeyboardEvent::KeyCode::Left) {
                if (cursor_position_ > 0) {
                    if (shift && !has_selection()) {
                        selection_start_ = cursor_position_;
                    }
                    cursor_position_--;
                    if (shift) {
                        selection_end_ = cursor_position_;
                    } else {
                        clear_selection();
                    }
                }
                cursor_blink_time_ = 0.0f;
                cursor_visible_ = true;
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::Right) {
                if (cursor_position_ < text_.length()) {
                    if (shift && !has_selection()) {
                        selection_start_ = cursor_position_;
                    }
                    cursor_position_++;
                    if (shift) {
                        selection_end_ = cursor_position_;
                    } else {
                        clear_selection();
                    }
                }
                cursor_blink_time_ = 0.0f;
                cursor_visible_ = true;
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::Home) {
                if (shift && !has_selection()) {
                    selection_start_ = cursor_position_;
                }
                cursor_position_ = 0;
                if (shift) {
                    selection_end_ = cursor_position_;
                } else {
                    clear_selection();
                }
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::End) {
                if (shift && !has_selection()) {
                    selection_start_ = cursor_position_;
                }
                cursor_position_ = text_.length();
                if (shift) {
                    selection_end_ = cursor_position_;
                } else {
                    clear_selection();
                }
                handled = true;
            }
            else if (key == KeyboardEvent::KeyCode::Back) {
                if (!read_only_) {
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
            }
            else if (key == KeyboardEvent::KeyCode::Delete) {
                if (!read_only_) {
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
            }
            else if (key == KeyboardEvent::KeyCode::Enter) {
                if (on_enter_pressed_) {
                    on_enter_pressed_(this);
                }
                handled = true;
            }
            // Character input - use proper conversion
            else if (!read_only_) {
                wchar_t ch = vk_to_char(key, shift);
                if (ch != 0 && ch >= 32) {  // Printable character
                    insert_character(ch);
                    handled = true;
                }
            }

            if (handled) {
                mark_dirty();
            }

            return handled || Widget::handle_key_down(event);
        }

        // Setters
        void set_text(const std::wstring& text) {
            if (text_ != text) {
                text_ = text.substr(0, max_length_);
                cursor_position_ = text_.length();
                clamp_cursor();
                clear_selection();
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
        
        void set_read_only(bool read_only) {
            read_only_ = read_only;
        }

        void on_text_changed(std::function<void(TextBox*, const std::wstring&)> callback) {
            on_text_changed_ = std::move(callback);
        }

        void on_enter_pressed(std::function<void(TextBox*)> callback) {
            on_enter_pressed_ = std::move(callback);
        }

        // Getters
        const std::wstring& get_text() const noexcept { return text_; }
        bool is_password_mode() const noexcept { return is_password_; }
        size_t get_max_length() const noexcept { return max_length_; }
        bool is_read_only() const noexcept { return read_only_; }
    };

} // namespace zuu::widget