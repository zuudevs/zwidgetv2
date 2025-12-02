#pragma once

#include "container.hpp"
#include "button.hpp"

namespace zuu::widget {

    class ComboBoxItem {
    public:
        std::wstring text;
        void* user_data{nullptr};
        
        ComboBoxItem() = default;
        explicit ComboBoxItem(const std::wstring& t) : text(t) {}
        ComboBoxItem(const std::wstring& t, void* data) : text(t), user_data(data) {}
    };

    // Internal dropdown list widget
    class DropdownList : public Container {
    private:
        std::vector<ComboBoxItem> items_;
        int selected_index_{-1};
        int hovered_index_{-1};
        float item_height_{30.0f};
        
        Color item_bg_normal_{Color::from_hex(0x2d2d2d)};
        Color item_bg_hover_{Color::from_hex(0x3d3d3d)};
        Color item_bg_selected_{Color::from_hex(0x4a90e2)};
        
        std::function<void(int)> on_item_selected_;
        
    public:
        DropdownList() {
            style_.background_color = Color::from_hex(0x2d2d2d);
            style_.border_color = Color::from_hex(0x4a90e2);
            style_.border_width = 1.0f;
            style_.padding = {2.0f, 2.0f, 2.0f, 2.0f};
        }
        
        void set_items(const std::vector<ComboBoxItem>& items) {
            items_ = items;
            update_size();
            mark_dirty();
        }
        
        void set_selected_index(int index) {
            if (index >= -1 && index < static_cast<int>(items_.size())) {
                selected_index_ = index;
                mark_dirty();
            }
        }
        
        void on_item_selected(std::function<void(int)> callback) {
            on_item_selected_ = std::move(callback);
        }
        
        void update_size() {
            float height = items_.size() * item_height_ + 
                          style_.padding.top + style_.padding.bottom;
            set_size(basic_size<float>(bounds_.w, height));
        }
        
        void render(Renderer& renderer) override {
            if (!is_visible()) return;
            
            // Background and border
            renderer.fill_rect(bounds_, style_.background_color);
            renderer.draw_rect(bounds_, style_.border_color, style_.border_width);
            
            // Draw items
            float y = content_bounds_.y;
            
            for (size_t i = 0; i < items_.size(); ++i) {
                basic_rect<float> item_rect(
                    content_bounds_.x,
                    y,
                    content_bounds_.w,
                    item_height_
                );
                
                // Background
                Color bg = item_bg_normal_;
                if (static_cast<int>(i) == selected_index_) {
                    bg = item_bg_selected_;
                } else if (static_cast<int>(i) == hovered_index_) {
                    bg = item_bg_hover_;
                }
                renderer.fill_rect(item_rect, bg);
                
                // Text
                Color text_color = (static_cast<int>(i) == selected_index_) 
                    ? Color::White() 
                    : Color::LightGray();
                    
                auto text_rect = item_rect;
                text_rect.x += 8.0f;
                text_rect.w -= 16.0f;
                renderer.draw_text(items_[i].text, text_rect, text_color);
                
                y += item_height_;
            }
            
            set_flag(WidgetFlag::Dirty, false);
        }
        
        bool handle_mouse_move(const MouseEvent& event) override {
            auto pos = basic_point<float>(
                static_cast<float>(event.get_position().x),
                static_cast<float>(event.get_position().y)
            );
            
            int old_hover = hovered_index_;
            hovered_index_ = -1;
            
            if (contains_point(pos)) {
                float relative_y = pos.y - content_bounds_.y;
                int index = static_cast<int>(relative_y / item_height_);
                
                if (index >= 0 && index < static_cast<int>(items_.size())) {
                    hovered_index_ = index;
                }
            }
            
            if (old_hover != hovered_index_) {
                mark_dirty();
            }
            
            return true;
        }
        
        bool handle_mouse_down(const MouseEvent& event) override {
            if (event.get_button() != MouseEvent::Button::left) {
                return false;
            }
            
            auto pos = basic_point<float>(
                static_cast<float>(event.get_position().x),
                static_cast<float>(event.get_position().y)
            );
            
            if (contains_point(pos)) {
                float relative_y = pos.y - content_bounds_.y;
                int index = static_cast<int>(relative_y / item_height_);
                
                if (index >= 0 && index < static_cast<int>(items_.size())) {
                    selected_index_ = index;
                    
                    if (on_item_selected_) {
                        on_item_selected_(index);
                    }
                    
                    mark_dirty();
                    return true;
                }
            }
            
            return false;
        }
        
        int get_selected_index() const { return selected_index_; }
        const std::vector<ComboBoxItem>& get_items() const { return items_; }
    };

    class ComboBox : public Widget {
    private:
        std::vector<ComboBoxItem> items_;
        int selected_index_{-1};
        bool is_open_{false};
        
        std::unique_ptr<DropdownList> dropdown_;
        Window* parent_window_{nullptr};
        
        Color button_bg_normal_{Color::from_hex(0x3a3a3a)};
        Color button_bg_hover_{Color::from_hex(0x454545)};
        Color arrow_color_{Color::White()};
        
        std::function<void(ComboBox*, int)> on_selection_changed_;
        
        void open_dropdown() {
            if (is_open_ || items_.empty()) return;
            
            is_open_ = true;
            
            if (!dropdown_) {
                dropdown_ = std::make_unique<DropdownList>();
                dropdown_->set_items(items_);
                dropdown_->set_selected_index(selected_index_);
                
                dropdown_->on_item_selected([this](int index) {
                    set_selected_index(index);
                    close_dropdown();
                });
            }
            
            // Position dropdown below combobox
            dropdown_->set_bounds(basic_rect<float>(
                bounds_.x,
                bounds_.y + bounds_.h,
                bounds_.w,
                200.0f  // Will be adjusted by dropdown
            ));
            dropdown_->update_size();
            dropdown_->set_visible(true);
            
            mark_dirty();
        }
        
        void close_dropdown() {
            if (!is_open_) return;
            
            is_open_ = false;
            if (dropdown_) {
                dropdown_->set_visible(false);
            }
            
            mark_dirty();
        }
        
    public:
        ComboBox() {
            set_focusable(true);
            style_.border_width = 1.0f;
            style_.border_radius = 3.0f;
            style_.padding = {8.0f, 5.0f, 28.0f, 5.0f};  // Extra padding for arrow
            style_.text_color = Color::White();
            style_.border_color = Color::Gray();
        }
        
        void render(Renderer& renderer) override {
            if (!is_visible()) return;
            
            // Button background
            Color bg = is_hovered() ? button_bg_hover_ : button_bg_normal_;
            
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
            
            // Selected item text
            if (selected_index_ >= 0 && selected_index_ < static_cast<int>(items_.size())) {
                renderer.draw_text(
                    items_[selected_index_].text,
                    content_bounds_,
                    style_.text_color
                );
            }
            
            // Dropdown arrow
            float arrow_x = bounds_.x + bounds_.w - 20.0f;
            float arrow_y = bounds_.y + bounds_.h * 0.5f;
            float arrow_size = 6.0f;
            
            // Draw triangle (simplified)
            if (is_open_) {
                // Up arrow
                renderer.draw_line(
                    basic_point<float>(arrow_x - arrow_size, arrow_y + arrow_size * 0.3f),
                    basic_point<float>(arrow_x, arrow_y - arrow_size * 0.5f),
                    arrow_color_,
                    2.0f
                );
                renderer.draw_line(
                    basic_point<float>(arrow_x, arrow_y - arrow_size * 0.5f),
                    basic_point<float>(arrow_x + arrow_size, arrow_y + arrow_size * 0.3f),
                    arrow_color_,
                    2.0f
                );
            } else {
                // Down arrow
                renderer.draw_line(
                    basic_point<float>(arrow_x - arrow_size, arrow_y - arrow_size * 0.3f),
                    basic_point<float>(arrow_x, arrow_y + arrow_size * 0.5f),
                    arrow_color_,
                    2.0f
                );
                renderer.draw_line(
                    basic_point<float>(arrow_x, arrow_y + arrow_size * 0.5f),
                    basic_point<float>(arrow_x + arrow_size, arrow_y - arrow_size * 0.3f),
                    arrow_color_,
                    2.0f
                );
            }
            
            set_flag(WidgetFlag::Dirty, false);
            
            // Render dropdown if open
            if (is_open_ && dropdown_) {
                dropdown_->render(renderer);
            }
        }
        
        bool handle_mouse_down(const MouseEvent& event) override {
            if (!is_enabled()) return false;
            
            if (event.get_button() == MouseEvent::Button::left) {
                auto pos = basic_point<float>(
                    static_cast<float>(event.get_position().x),
                    static_cast<float>(event.get_position().y)
                );
                
                if (is_open_ && dropdown_ && dropdown_->contains_point(pos)) {
                    return dropdown_->handle_mouse_down(event);
                }
                
                if (contains_point(pos)) {
                    if (is_open_) {
                        close_dropdown();
                    } else {
                        open_dropdown();
                    }
                    return true;
                }
                
                // Click outside closes dropdown
                if (is_open_) {
                    close_dropdown();
                }
            }
            
            return Widget::handle_mouse_down(event);
        }
        
        bool handle_mouse_move(const MouseEvent& event) override {
            if (is_open_ && dropdown_) {
                dropdown_->handle_mouse_move(event);
            }
            return Widget::handle_mouse_move(event);
        }
        
        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_enabled() || !is_focused()) return false;
            
            bool handled = false;
            
            if (event.get_key() == KeyboardEvent::KeyCode::Space ||
                event.get_key() == KeyboardEvent::KeyCode::Enter) {
                if (is_open_) {
                    close_dropdown();
                } else {
                    open_dropdown();
                }
                handled = true;
            }
            else if (event.get_key() == KeyboardEvent::KeyCode::Escape) {
                if (is_open_) {
                    close_dropdown();
                    handled = true;
                }
            }
            else if (event.get_key() == KeyboardEvent::KeyCode::Up) {
                if (selected_index_ > 0) {
                    set_selected_index(selected_index_ - 1);
                    handled = true;
                }
            }
            else if (event.get_key() == KeyboardEvent::KeyCode::Down) {
                if (selected_index_ < static_cast<int>(items_.size()) - 1) {
                    set_selected_index(selected_index_ + 1);
                    handled = true;
                }
            }
            
            return handled || Widget::handle_key_down(event);
        }
        
        // Item management
        void add_item(const std::wstring& text) {
            items_.emplace_back(text);
            if (dropdown_) {
                dropdown_->set_items(items_);
            }
            mark_dirty();
        }
        
        void add_item(const std::wstring& text, void* user_data) {
            items_.emplace_back(text, user_data);
            if (dropdown_) {
                dropdown_->set_items(items_);
            }
            mark_dirty();
        }
        
        void clear_items() {
            items_.clear();
            selected_index_ = -1;
            if (dropdown_) {
                dropdown_->set_items(items_);
            }
            mark_dirty();
        }
        
        void set_selected_index(int index) {
            if (index >= -1 && index < static_cast<int>(items_.size())) {
                if (selected_index_ != index) {
                    selected_index_ = index;
                    
                    if (dropdown_) {
                        dropdown_->set_selected_index(index);
                    }
                    
                    mark_dirty();
                    
                    if (on_selection_changed_) {
                        on_selection_changed_(this, index);
                    }
                }
            }
        }
        
        void on_selection_changed(std::function<void(ComboBox*, int)> callback) {
            on_selection_changed_ = std::move(callback);
        }
        
        // Getters
        int get_selected_index() const { return selected_index_; }
        
        const ComboBoxItem* get_selected_item() const {
            if (selected_index_ >= 0 && selected_index_ < static_cast<int>(items_.size())) {
                return &items_[selected_index_];
            }
            return nullptr;
        }
        
        const std::vector<ComboBoxItem>& get_items() const { return items_; }
        bool is_open() const { return is_open_; }
    };

} // namespace zuu::widget