#pragma once

#include "widget.hpp"
#include <algorithm>
#include <memory>

namespace zuu::widget {

    // Container - widget yang bisa memiliki children
    class Container : public Widget {
    protected:
        std::vector<std::unique_ptr<Widget>> children_;
        Widget* focused_child_{nullptr};
        Widget* hovered_child_{nullptr};

    public:
        Container() = default;
        virtual ~Container() = default;

        // Child management
        template<typename T, typename... Args>
        T* add_child(Args&&... args) {
            auto widget = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = widget.get();
            widget->parent_ = this;
            children_.push_back(std::move(widget));
            set_flag(WidgetFlag::LayoutDirty, true);
            mark_dirty();
            return ptr;
        }

        void add_child(std::unique_ptr<Widget> widget) {
            if (!widget) return;
            widget->parent_ = this;
            children_.push_back(std::move(widget));
            set_flag(WidgetFlag::LayoutDirty, true);
            mark_dirty();
        }

        void remove_child(Widget* widget) {
            auto it = std::find_if(children_.begin(), children_.end(),
                [widget](const auto& child) { return child.get() == widget; });
            
            if (it != children_.end()) {
                if (focused_child_ == widget) {
                    focused_child_ = nullptr;
                }
                if (hovered_child_ == widget) {
                    hovered_child_ = nullptr;
                }
                children_.erase(it);
                set_flag(WidgetFlag::LayoutDirty, true);
                mark_dirty();
            }
        }

        void clear_children() {
            children_.clear();
            focused_child_ = nullptr;
            hovered_child_ = nullptr;
            set_flag(WidgetFlag::LayoutDirty, true);
            mark_dirty();
        }

        Widget* find_child_by_id(const std::string& id) {
            for (auto& child : children_) {
                if (child->get_id() == id) {
                    return child.get();
                }
                if (auto* container = dynamic_cast<Container*>(child.get())) {
                    if (auto* found = container->find_child_by_id(id)) {
                        return found;
                    }
                }
            }
            return nullptr;
        }

        // Hit testing dengan children
        Widget* find_widget_at(const basic_point<float>& point) {
            if (!is_visible() || !is_enabled()) return nullptr;
            if (!contains_point(point)) return nullptr;

            // Test children in reverse order (top to bottom)
            for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
                auto& child = *it;
                
                if (auto* container = dynamic_cast<Container*>(child.get())) {
                    if (auto* found = container->find_widget_at(point)) {
                        return found;
                    }
                } else if (child->contains_point(point)) {
                    return child.get();
                }
            }

            return this;
        }

        // Rendering
        void render(Renderer& renderer) override {
            if (!is_visible()) return;

            // Render self
            Widget::render(renderer);

            // Render children
            for (auto& child : children_) {
                child->render(renderer);
            }
        }

        // Update
        void update(float dt) override {
            Widget::update(dt);
            for (auto& child : children_) {
                child->update(dt);
            }
        }

        // Layout
        void layout() override {
            Widget::layout();
            
            // Layout children - override in subclasses for specific layouts
            for (auto& child : children_) {
                if (child->needs_layout()) {
                    child->layout();
                }
            }
        }

        // Event handling dengan propagation
        bool handle_mouse_down(const MouseEvent& event) override {
            if (!is_visible() || !is_enabled()) return false;

            auto pos = basic_point<float>(
                static_cast<float>(event.get_position().x),
                static_cast<float>(event.get_position().y)
            );

            Widget* target = find_widget_at(pos);
            
            if (target && target != this) {
                return target->handle_mouse_down(event);
            }

            return Widget::handle_mouse_down(event);
        }

        bool handle_mouse_up(const MouseEvent& event) override {
            if (!is_visible() || !is_enabled()) return false;

            auto pos = basic_point<float>(
                static_cast<float>(event.get_position().x),
                static_cast<float>(event.get_position().y)
            );

            Widget* target = find_widget_at(pos);
            
            if (target && target != this) {
                return target->handle_mouse_up(event);
            }

            return Widget::handle_mouse_up(event);
        }

        bool handle_mouse_move(const MouseEvent& event) override {
            if (!is_visible() || !is_enabled()) return false;

            auto pos = basic_point<float>(
                static_cast<float>(event.get_position().x),
                static_cast<float>(event.get_position().y)
            );

            Widget* target = find_widget_at(pos);

            // Handle hover state changes
            if (target != hovered_child_) {
                if (hovered_child_) {
                    hovered_child_->set_hovered(false);
                }
                hovered_child_ = target;
                if (hovered_child_ && hovered_child_ != this) {
                    hovered_child_->set_hovered(true);
                }
            }

            if (target && target != this) {
                return target->handle_mouse_move(event);
            }

            return Widget::handle_mouse_move(event);
        }

        bool handle_key_down(const KeyboardEvent& event) override {
            if (!is_visible() || !is_enabled()) return false;

            // Send to focused child first
            if (focused_child_ && focused_child_->handle_key_down(event)) {
                return true;
            }

            return Widget::handle_key_down(event);
        }

        bool handle_key_up(const KeyboardEvent& event) override {
            if (!is_visible() || !is_enabled()) return false;

            // Send to focused child first
            if (focused_child_ && focused_child_->handle_key_up(event)) {
                return true;
            }

            return Widget::handle_key_up(event);
        }

        // Focus management
        void set_focused_child(Widget* child) {
            if (focused_child_ == child) return;

            if (focused_child_) {
                focused_child_->set_focused(false);
            }

            focused_child_ = child;

            if (focused_child_) {
                focused_child_->set_focused(true);
            }
        }

        Widget* get_focused_child() const noexcept {
            return focused_child_;
        }

        // Getters
        const std::vector<std::unique_ptr<Widget>>& get_children() const noexcept {
            return children_;
        }

        size_t child_count() const noexcept {
            return children_.size();
        }
    };

} // namespace zuu::widget