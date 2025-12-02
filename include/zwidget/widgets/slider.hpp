#pragma once

#include "widget.hpp"
#include <algorithm>
#include <functional>
#include <cmath>

namespace zuu::widget {

    enum class SliderOrientation {
        Horizontal,
        Vertical
    };

    class Slider : public Widget {
    private:
        float min_value_{0.0f};
        float max_value_{100.0f};
        float current_value_{0.0f};
        float step_{1.0f};
        SliderOrientation orientation_{SliderOrientation::Horizontal};
        
        // Visual properties
        float track_thickness_{4.0f};
        float thumb_size_{16.0f};
        
        Color track_color_{Color::from_hex(0x4a4a4a)};
        Color track_fill_color_{Color::from_hex(0x4a90e2)};
        Color thumb_color_{Color::from_hex(0xffffff)};
        Color thumb_hover_color_{Color::from_hex(0xe0e0e0)};
        Color thumb_active_color_{Color::from_hex(0xc0c0c0)};
        
        bool is_dragging_{false};
        std::function<void(Slider*, float)> on_value_changed_;
        
        float get_normalized_value() const {
            if (max_value_ <= min_value_) return 0.0f;
            return (current_value_ - min_value_) / (max_value_ - min_value_);
        }
        
        float get_thumb_position() const {
            float normalized = get_normalized_value();
            
            if (orientation_ == SliderOrientation::Horizontal) {
                float track_length = content_bounds_.w - thumb_size_;
                return content_bounds_.x + thumb_size_ * 0.5f + normalized * track_length;
            } else {
                float track_length = content_bounds_.h - thumb_size_;
                return content_bounds_.y + thumb_size_ * 0.5f + normalized * track_length;
            }
        }
        
        basic_rect<float> get_thumb_rect() const {
            float thumb_pos = get_thumb_position();
            
            if (orientation_ == SliderOrientation::Horizontal) {
                float y = content_bounds_.y + (content_bounds_.h - thumb_size_) * 0.5f;
                return basic_rect<float>(
                    thumb_pos - thumb_size_ * 0.5f,
                    y,
                    thumb_size_,
                    thumb_size_
                );
            } else {
                float x = content_bounds_.x + (content_bounds_.w - thumb_size_) * 0.5f;
                return basic_rect<float>(
                    x,
                    thumb_pos - thumb_size_ * 0.5f,
                    thumb_size_,
                    thumb_size_
                );
            }
        }
        
        bool is_point_in_thumb(const basic_point<float>& point) const {
            auto thumb_rect = get_thumb_rect();
            return point.x >= thumb_rect.x && point.x <= thumb_rect.x + thumb_rect.w &&
                   point.y >= thumb_rect.y && point.y <= thumb_rect.y + thumb_rect.h;
        }
        
        void update_value_from_position(const basic_point<float>& pos) {
            float normalized;
            
            if (orientation_ == SliderOrientation::Horizontal) {
                float track_start = content_bounds_.x + thumb_size_ * 0.5f;
                float track_length = content_bounds_.w - thumb_size_;
                normalized = (pos.x - track_start) / track_length;
            } else {
                float track_start = content_bounds_.y + thumb_size_ * 0.5f;
                float track_length = content_bounds_.h - thumb_size_;
                normalized = (pos.y - track_start) / track_length;
            }
            
            // Clamp and apply step
            normalized = std::clamp(normalized, 0.0f, 1.0f);
            float raw_value = min_value_ + normalized * (max_value_ - min_value_);
            
            if (step_ > 0) {
                raw_value = std::round(raw_value / step_) * step_;
            }
            
            set_value(raw_value);
        }

    public:
        Slider() {
            set_focusable(true);
            style_.padding = {8.0f, 8.0f, 8.0f, 8.0f};
        }
        
        explicit Slider(SliderOrientation orientation) : Slider() {
            orientation_ = orientation;
        }
        
        void render(Renderer& renderer) override {
            if (!is_visible()) return;
            
            // Draw background
            Widget::render(renderer);
            
            // Calculate track rect
            basic_rect<float> track_rect;
            if (orientation_ == SliderOrientation::Horizontal) {
                float y = content_bounds_.y + (content_bounds_.h - track_thickness_) * 0.5f;
                track_rect = basic_rect<float>(
                    content_bounds_.x + thumb_size_ * 0.5f,
                    y,
                    content_bounds_.w - thumb_size_,
                    track_thickness_
                );
            } else {
                float x = content_bounds_.x + (content_bounds_.w - track_thickness_) * 0.5f;
                track_rect = basic_rect<float>(
                    x,
                    content_bounds_.y + thumb_size_ * 0.5f,
                    track_thickness_,
                    content_bounds_.h - thumb_size_
                );
            }
            
            // Draw track background
            renderer.fill_rounded_rect(
                track_rect,
                track_thickness_ * 0.5f,
                track_thickness_ * 0.5f,
                track_color_
            );
            
            // Draw filled portion of track
            float normalized = get_normalized_value();
            basic_rect<float> fill_rect = track_rect;
            
            if (orientation_ == SliderOrientation::Horizontal) {
                fill_rect.w *= normalized;
            } else {
                fill_rect.h *= normalized;
            }
            
            if (fill_rect.w > 0 && fill_rect.h > 0) {
                renderer.fill_rounded_rect(
                    fill_rect,
                    track_thickness_ * 0.5f,
                    track_thickness_ * 0.5f,
                    track_fill_color_
                );
            }
            
            // Draw thumb
            auto thumb_rect = get_thumb_rect();
            Color thumb_color = thumb_color_;
            
            if (!is_enabled()) {
                thumb_color = Color(0.5f, 0.5f, 0.5f, 0.5f);
            } else if (is_dragging_) {
                thumb_color = thumb_active_color_;
            } else if (is_hovered()) {
                thumb_color = thumb_hover_color_;
            }
            
            renderer.fill_circle(
                basic_point<float>(
                    thumb_rect.x + thumb_rect.w * 0.5f,
                    thumb_rect.y + thumb_rect.h * 0.5f
                ),
                thumb_size_ * 0.5f,
                thumb_color
            );
            
            // Draw focus indicator
            if (is_focused()) {
                renderer.draw_circle(
                    basic_point<float>(
                        thumb_rect.x + thumb_rect.w * 0.5f,
                        thumb_rect.y + thumb_rect.h * 0.5f
                    ),
                    thumb_size_ * 0.5f + 2.0f,
                    Color::from_hex(0x4a90e2),
                    2.0f
                );
            }
            
            set_flag(WidgetFlag::Dirty, false);
        }
        
        bool handle_mouse_down(const MouseEvent& event) override {
            if (!is_enabled()) return false;
            
            if (event.get_button() == MouseEvent::Button::left) {
                auto pos = basic_point<float>(
                    static_cast<float>(event.get_position().x),
                    static_cast<float>(event.get_position().y)
                );
                
                if (is_point_in_thumb(pos) || contains_point(pos)) {
                    is_dragging_ = true;
                    set_pressed(true);
                    update_value_from_position(pos);
                    return true;
                }
            }
            
            return Widget::handle_mouse_down(event);
        }
        
        bool handle_mouse_up(const MouseEvent& event) override {
            if (!is_enabled()) return false;
            
            if (event.get_button() == MouseEvent::Button::left && is_dragging_) {
                is_dragging_ = false;
                set_pressed(false);
                return true;
            }
            
            return Widget::handle_mouse_up(event);
        }
        
        bool handle_mouse_move(const MouseEvent& event) override {
            if (!is_enabled()) return false;
            
            if (is_dragging_) {
                auto pos = basic_point<float>(
                    static_cast<float>(event.get_position().x),
                    static_cast<float>(event.get_position().y)
                );
                update_value_from_position(pos);
                return true;
            }
            
            return Widget::handle_mouse_move(event);
        }
        
        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_enabled()) return false;
            
            bool handled = false;
            float increment = step_ > 0 ? step_ : (max_value_ - min_value_) * 0.01f;
            
            if (orientation_ == SliderOrientation::Horizontal) {
                if (event.get_key() == KeyboardEvent::KeyCode::Left) {
                    set_value(current_value_ - increment);
                    handled = true;
                } else if (event.get_key() == KeyboardEvent::KeyCode::Right) {
                    set_value(current_value_ + increment);
                    handled = true;
                }
            } else {
                if (event.get_key() == KeyboardEvent::KeyCode::Up) {
                    set_value(current_value_ + increment);
                    handled = true;
                } else if (event.get_key() == KeyboardEvent::KeyCode::Down) {
                    set_value(current_value_ - increment);
                    handled = true;
                }
            }
            
            if (event.get_key() == KeyboardEvent::KeyCode::Home) {
                set_value(min_value_);
                handled = true;
            } else if (event.get_key() == KeyboardEvent::KeyCode::End) {
                set_value(max_value_);
                handled = true;
            } else if (event.get_key() == KeyboardEvent::KeyCode::PageUp) {
                set_value(current_value_ + increment * 10);
                handled = true;
            } else if (event.get_key() == KeyboardEvent::KeyCode::PageDown) {
                set_value(current_value_ - increment * 10);
                handled = true;
            }
            
            return handled || Widget::handle_key_down(event);
        }
        
        // Setters
        void set_range(float min_val, float max_val) {
            min_value_ = min_val;
            max_value_ = max_val;
            set_value(current_value_); // Reclamp
            mark_dirty();
        }
        
        void set_value(float value) {
            float clamped = std::clamp(value, min_value_, max_value_);
            
            if (clamped != current_value_) {
                current_value_ = clamped;
                mark_dirty();
                
                if (on_value_changed_) {
                    on_value_changed_(this, current_value_);
                }
            }
        }
        
        void set_step(float step) {
            step_ = step;
        }
        
        void set_orientation(SliderOrientation orientation) {
            if (orientation_ != orientation) {
                orientation_ = orientation;
                mark_dirty();
            }
        }
        
        void on_value_changed(std::function<void(Slider*, float)> callback) {
            on_value_changed_ = std::move(callback);
        }
        
        // Getters
        float get_value() const noexcept { return current_value_; }
        float get_min_value() const noexcept { return min_value_; }
        float get_max_value() const noexcept { return max_value_; }
        float get_step() const noexcept { return step_; }
        SliderOrientation get_orientation() const noexcept { return orientation_; }
    };

} // namespace zuu::widget