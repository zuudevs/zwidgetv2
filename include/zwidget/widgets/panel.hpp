#pragma once

#include "container.hpp"

namespace zuu::widget {

    // Layout direction
    enum class LayoutDirection {
        Horizontal,
        Vertical
    };

    // Base Panel - simple container
    class Panel : public Container {
    public:
        Panel() {
            style_.background_color = Color::from_hex(0x2d2d2d);
            style_.border_color = Color::from_hex(0x3d3d3d);
            style_.border_width = 1.0f;
        }
    };

    // Stack Panel - linear layout
    class StackPanel : public Container {
    private:
        LayoutDirection direction_{LayoutDirection::Vertical};
        float spacing_{5.0f};

    public:
        StackPanel() {
            style_.background_color = Color::Transparent();
        }

        explicit StackPanel(LayoutDirection dir) : StackPanel() {
            direction_ = dir;
        }

        void layout() override {
            Widget::layout();

            if (children_.empty()) return;

            float current_pos = 0.0f;

            if (direction_ == LayoutDirection::Vertical) {
                // Vertical layout
                float x = content_bounds_.x;
                float y = content_bounds_.y;
                float available_width = content_bounds_.w;

                for (auto& child : children_) {
                    if (!child->is_visible()) continue;

                    auto child_bounds = child->get_bounds();
                    
                    // Set position
                    child->set_position(basic_point<float>(x, y + current_pos));
                    
                    // Set width to fill (optional - could be configurable)
                    child->set_size(basic_size<float>(
                        available_width,
                        child_bounds.h
                    ));

                    current_pos += child_bounds.h + spacing_;

                    if (child->needs_layout()) {
                        child->layout();
                    }
                }
            } else {
                // Horizontal layout
                float x = content_bounds_.x;
                float y = content_bounds_.y;
                float available_height = content_bounds_.h;

                for (auto& child : children_) {
                    if (!child->is_visible()) continue;

                    auto child_bounds = child->get_bounds();
                    
                    // Set position
                    child->set_position(basic_point<float>(x + current_pos, y));
                    
                    // Set height to fill (optional - could be configurable)
                    child->set_size(basic_size<float>(
                        child_bounds.w,
                        available_height
                    ));

                    current_pos += child_bounds.w + spacing_;

                    if (child->needs_layout()) {
                        child->layout();
                    }
                }
            }
        }

        // Setters
        void set_direction(LayoutDirection dir) {
            if (direction_ != dir) {
                direction_ = dir;
                set_flag(WidgetFlag::LayoutDirty, true);
                mark_dirty();
            }
        }

        void set_spacing(float spacing) {
            if (spacing_ != spacing) {
                spacing_ = spacing;
                set_flag(WidgetFlag::LayoutDirty, true);
                mark_dirty();
            }
        }

        // Getters
        LayoutDirection get_direction() const noexcept {
            return direction_;
        }

        float get_spacing() const noexcept {
            return spacing_;
        }
    };

    // Grid Panel - grid layout
    class GridPanel : public Container {
    private:
        size_t rows_{1};
        size_t columns_{1};
        float h_spacing_{5.0f};
        float v_spacing_{5.0f};

    public:
        GridPanel() {
            style_.background_color = Color::Transparent();
        }

        GridPanel(size_t rows, size_t columns) : GridPanel() {
            rows_ = rows;
            columns_ = columns;
        }

        void layout() override {
            Widget::layout();

            if (children_.empty() || rows_ == 0 || columns_ == 0) return;

            float cell_width = (content_bounds_.w - (columns_ - 1) * h_spacing_) / columns_;
            float cell_height = (content_bounds_.h - (rows_ - 1) * v_spacing_) / rows_;

            size_t index = 0;
            for (auto& child : children_) {
                if (!child->is_visible()) continue;
                if (index >= rows_ * columns_) break;

                size_t row = index / columns_;
                size_t col = index % columns_;

                float x = content_bounds_.x + col * (cell_width + h_spacing_);
                float y = content_bounds_.y + row * (cell_height + v_spacing_);

                child->set_bounds(basic_rect<float>(x, y, cell_width, cell_height));

                if (child->needs_layout()) {
                    child->layout();
                }

                index++;
            }
        }

        // Setters
        void set_grid_size(size_t rows, size_t columns) {
            if (rows_ != rows || columns_ != columns) {
                rows_ = rows;
                columns_ = columns;
                set_flag(WidgetFlag::LayoutDirty, true);
                mark_dirty();
            }
        }

        void set_spacing(float h_spacing, float v_spacing) {
            if (h_spacing_ != h_spacing || v_spacing_ != v_spacing) {
                h_spacing_ = h_spacing;
                v_spacing_ = v_spacing;
                set_flag(WidgetFlag::LayoutDirty, true);
                mark_dirty();
            }
        }

        // Getters
        size_t get_rows() const noexcept { return rows_; }
        size_t get_columns() const noexcept { return columns_; }
        float get_horizontal_spacing() const noexcept { return h_spacing_; }
        float get_vertical_spacing() const noexcept { return v_spacing_; }
    };

} // namespace zuu::widget