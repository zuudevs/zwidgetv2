#pragma once

#include "container.hpp"

namespace zuu::widget {

    class CheckBox : public Widget {
    private:
        std::wstring label_;
        bool checked_{false};
        
        float box_size_{20.0f};
        float label_spacing_{8.0f};
        
        Color box_color_{Color::from_hex(0x4a4a4a)};
        Color check_color_{Color::from_hex(0x4a90e2)};
        Color hover_color_{Color::from_hex(0x5a5a5a)};
        
        std::function<void(CheckBox*, bool)> on_changed_;

    public:
        CheckBox() {
            set_focusable(true);
            style_.padding = {0, 0, 0, 0};
        }

        explicit CheckBox(const std::wstring& label) : CheckBox() {
            label_ = label;
        }

        void render(Renderer& renderer) override {
            if (!is_visible()) return;

            float box_x = bounds_.x;
            float box_y = bounds_.y + (bounds_.h - box_size_) * 0.5f;
            
            // Draw checkbox box
            Color bg = is_hovered() ? hover_color_ : box_color_;
            
            renderer.fill_rounded_rect(
                basic_rect<float>(box_x, box_y, box_size_, box_size_),
                3.0f, 3.0f,
                bg
            );

            // Draw focus border
            if (is_focused()) {
                renderer.draw_rounded_rect(
                    basic_rect<float>(box_x, box_y, box_size_, box_size_),
                    3.0f, 3.0f,
                    Color::from_hex(0x4a90e2),
                    2.0f
                );
            } else {
                renderer.draw_rounded_rect(
                    basic_rect<float>(box_x, box_y, box_size_, box_size_),
                    3.0f, 3.0f,
                    Color::Gray(),
                    1.0f
                );
            }

            // Draw check mark if checked
            if (checked_) {
                float padding = box_size_ * 0.25f;
                float check_x = box_x + padding;
                float check_y = box_y + padding;
                float check_size = box_size_ - padding * 2;
                
                // Draw checkmark (simplified X pattern)
                renderer.draw_line(
                    basic_point<float>(check_x, check_y),
                    basic_point<float>(check_x + check_size, check_y + check_size),
                    check_color_,
                    3.0f
                );
                renderer.draw_line(
                    basic_point<float>(check_x + check_size, check_y),
                    basic_point<float>(check_x, check_y + check_size),
                    check_color_,
                    3.0f
                );
            }

            // Draw label
            if (!label_.empty()) {
                float label_x = box_x + box_size_ + label_spacing_;
                renderer.draw_text(
                    label_,
                    basic_rect<float>(label_x, bounds_.y, 
                                     bounds_.w - box_size_ - label_spacing_, bounds_.h),
                    style_.text_color
                );
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

                if (was_pressed && is_hovered()) {
                    toggle();
                }

                return true;
            }

            return Widget::handle_mouse_up(event);
        }

        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_enabled()) return false;

            if (event.get_key() == KeyboardEvent::KeyCode::Space ||
                event.get_key() == KeyboardEvent::KeyCode::Enter) {
                toggle();
                return true;
            }

            return Widget::handle_key_down(event);
        }

        // Methods
        void toggle() {
            set_checked(!checked_);
        }

        void set_checked(bool checked) {
            if (checked_ != checked) {
                checked_ = checked;
                mark_dirty();
                
                if (on_changed_) {
                    on_changed_(this, checked_);
                }
            }
        }

        void set_label(const std::wstring& label) {
            if (label_ != label) {
                label_ = label;
                mark_dirty();
            }
        }

        void on_changed(std::function<void(CheckBox*, bool)> callback) {
            on_changed_ = std::move(callback);
        }

        // Getters
        bool is_checked() const noexcept {
            return checked_;
        }

        const std::wstring& get_label() const noexcept {
            return label_;
        }
    };

    // RadioButton - similar to CheckBox but with group behavior
    class RadioButton : public Widget {
    private:
        std::wstring label_;
        bool checked_{false};
        std::string group_name_;
        
        float circle_size_{20.0f};
        float label_spacing_{8.0f};
        
        Color circle_color_{Color::from_hex(0x4a4a4a)};
        Color check_color_{Color::from_hex(0x4a90e2)};
        Color hover_color_{Color::from_hex(0x5a5a5a)};
        
        std::function<void(RadioButton*, bool)> on_changed_;

    public:
        RadioButton() {
            set_focusable(true);
            style_.padding = {0, 0, 0, 0};
        }

        RadioButton(const std::wstring& label, const std::string& group) 
            : RadioButton() {
            label_ = label;
            group_name_ = group;
        }

        void render(Renderer& renderer) override {
            if (!is_visible()) return;

            float circle_x = bounds_.x + circle_size_ * 0.5f;
            float circle_y = bounds_.y + bounds_.h * 0.5f;
            
            // Draw outer circle
            Color bg = is_hovered() ? hover_color_ : circle_color_;
            
            renderer.fill_circle(
                basic_point<float>(circle_x, circle_y),
                circle_size_ * 0.5f,
                bg
            );

            // Draw focus border
            if (is_focused()) {
                renderer.draw_circle(
                    basic_point<float>(circle_x, circle_y),
                    circle_size_ * 0.5f,
                    Color::from_hex(0x4a90e2),
                    2.0f
                );
            } else {
                renderer.draw_circle(
                    basic_point<float>(circle_x, circle_y),
                    circle_size_ * 0.5f,
                    Color::Gray(),
                    1.0f
                );
            }

            // Draw inner circle if checked
            if (checked_) {
                renderer.fill_circle(
                    basic_point<float>(circle_x, circle_y),
                    circle_size_ * 0.3f,
                    check_color_
                );
            }

            // Draw label
            if (!label_.empty()) {
                float label_x = bounds_.x + circle_size_ + label_spacing_;
                renderer.draw_text(
                    label_,
                    basic_rect<float>(label_x, bounds_.y, 
                                     bounds_.w - circle_size_ - label_spacing_, bounds_.h),
                    style_.text_color
                );
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

                if (was_pressed && is_hovered()) {
                    set_checked(true);
                }

                return true;
            }

            return Widget::handle_mouse_up(event);
        }

        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_enabled()) return false;

            if (event.get_key() == KeyboardEvent::KeyCode::Space ||
                event.get_key() == KeyboardEvent::KeyCode::Enter) {
                set_checked(true);
                return true;
            }

            return Widget::handle_key_down(event);
        }

        // Methods
        void set_checked(bool checked) {
            if (checked_ == checked) return;
            
            checked_ = checked;
            mark_dirty();
            
            // Uncheck other radio buttons in the same group
            if (checked_ && parent_ && !group_name_.empty()) {
                uncheck_group_siblings();
            }
            
            if (on_changed_) {
                on_changed_(this, checked_);
            }
        }

        void set_label(const std::wstring& label) {
            if (label_ != label) {
                label_ = label;
                mark_dirty();
            }
        }

        void set_group(const std::string& group) {
            group_name_ = group;
        }

        void on_changed(std::function<void(RadioButton*, bool)> callback) {
            on_changed_ = std::move(callback);
        }

        // Getters
        bool is_checked() const noexcept {
            return checked_;
        }

        const std::wstring& get_label() const noexcept {
            return label_;
        }

        const std::string& get_group() const noexcept {
            return group_name_;
        }

    private:
        void uncheck_group_siblings() {
            if (!parent_) return;
            
            auto* container = dynamic_cast<Container*>(parent_);
            if (!container) return;
            
            for (auto& child : container->get_children()) {
                auto* radio = dynamic_cast<RadioButton*>(child.get());
                if (radio && radio != this && radio->group_name_ == group_name_) {
                    radio->checked_ = false;
                    radio->mark_dirty();
                }
            }
        }
    };

} // namespace zuu::widget