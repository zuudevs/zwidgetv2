#pragma once

#include "zwidget/unit/rect.hpp"
#include "zwidget/unit/event.hpp"
#include "zwidget/graphic/renderer.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace zuu::widget {

    // Forward declarations
    class Widget;
    class Container;

    // Widget flags
    enum class WidgetFlag : uint32_t {
        None            = 0,
        Visible         = 1 << 0,
        Enabled         = 1 << 1,
        Focusable       = 1 << 2,
        Focused         = 1 << 3,
        Hovered         = 1 << 4,
        Pressed         = 1 << 5,
        Dirty           = 1 << 6,  // Needs redraw
        LayoutDirty     = 1 << 7,  // Needs layout recalc
    };

    constexpr WidgetFlag operator|(WidgetFlag a, WidgetFlag b) noexcept {
        return static_cast<WidgetFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr WidgetFlag operator&(WidgetFlag a, WidgetFlag b) noexcept {
        return static_cast<WidgetFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr bool has_flag(WidgetFlag flags, WidgetFlag check) noexcept {
        return (flags & check) == check;
    }

    // Widget style
    struct WidgetStyle {
        Color background_color{Color::Transparent()};
        Color border_color{Color::Gray()};
        Color text_color{Color::White()};
        
        float border_width{1.0f};
        float border_radius{0.0f};
        
        struct {
            float left{0.0f};
            float top{0.0f};
            float right{0.0f};
            float bottom{0.0f};
        } padding;
        
        struct {
            float left{0.0f};
            float top{0.0f};
            float right{0.0f};
            float bottom{0.0f};
        } margin;
    };

    // Base Widget class
    class Widget {
    protected:
        Container* parent_{nullptr};
        basic_rect<float> bounds_{0, 0, 100, 100};
        basic_rect<float> content_bounds_{0, 0, 100, 100};
        WidgetFlag flags_{WidgetFlag::Visible | WidgetFlag::Enabled};
        WidgetStyle style_;
        std::string id_;

        // Event callbacks
        std::function<void(Widget*, const MouseEvent&)> on_mouse_down_;
        std::function<void(Widget*, const MouseEvent&)> on_mouse_up_;
        std::function<void(Widget*, const MouseEvent&)> on_mouse_move_;
        std::function<void(Widget*, const MouseEvent&)> on_mouse_enter_;
        std::function<void(Widget*, const MouseEvent&)> on_mouse_leave_;
        std::function<void(Widget*, const KeyboardEvent&)> on_key_down_;
        std::function<void(Widget*, const KeyboardEvent&)> on_key_up_;
        std::function<void(Widget*)> on_focus_gained_;
        std::function<void(Widget*)> on_focus_lost_;

        void set_flag(WidgetFlag flag, bool value = true) noexcept {
            if (value) {
                flags_ = flags_ | flag;
            } else {
                flags_ = static_cast<WidgetFlag>(
                    static_cast<uint32_t>(flags_) & ~static_cast<uint32_t>(flag)
                );
            }
        }

        void mark_dirty() {
            set_flag(WidgetFlag::Dirty, true);
            if (parent_) {
                // Propagate to parent - implemented in container.hpp
            }
        }

        void update_content_bounds() {
            content_bounds_ = basic_rect<float>(
                bounds_.x + style_.padding.left,
                bounds_.y + style_.padding.top,
                bounds_.w - style_.padding.left - style_.padding.right,
                bounds_.h - style_.padding.top - style_.padding.bottom
            );
        }

    public:
        Widget() = default;
        virtual ~Widget() = default;

        Widget(const Widget&) = delete;
        Widget& operator=(const Widget&) = delete;
        Widget(Widget&&) = default;
        Widget& operator=(Widget&&) = default;

        // Core virtual methods
        virtual void render(Renderer& renderer) {
            if (!is_visible()) return;

            // Draw background
            if (style_.background_color.a() > 0) {
                if (style_.border_radius > 0) {
                    renderer.fill_rounded_rect(
                        bounds_,
                        style_.border_radius,
                        style_.border_radius,
                        style_.background_color
                    );
                } else {
                    renderer.fill_rect(bounds_, style_.background_color);
                }
            }

            // Draw border
            if (style_.border_width > 0 && style_.border_color.a() > 0) {
                if (style_.border_radius > 0) {
                    renderer.draw_rounded_rect(
                        bounds_,
                        style_.border_radius,
                        style_.border_radius,
                        style_.border_color,
                        style_.border_width
                    );
                } else {
                    renderer.draw_rect(bounds_, style_.border_color, style_.border_width);
                }
            }

            set_flag(WidgetFlag::Dirty, false);
        }

        virtual void update(float dt) {
            // Override for animations/logic
        }

        virtual void layout() {
            update_content_bounds();
            set_flag(WidgetFlag::LayoutDirty, false);
        }

        // Event handlers - return true if handled
        virtual bool handle_mouse_down(const MouseEvent& event) {
            if (on_mouse_down_) {
                on_mouse_down_(this, event);
                return true;
            }
            return false;
        }

        virtual bool handle_mouse_up(const MouseEvent& event) {
            if (on_mouse_up_) {
                on_mouse_up_(this, event);
                return true;
            }
            return false;
        }

        virtual bool handle_mouse_move(const MouseEvent& event) {
            if (on_mouse_move_) {
                on_mouse_move_(this, event);
                return true;
            }
            return false;
        }

        virtual bool handle_key_down(const KeyboardEvent& event) {
            if (on_key_down_) {
                on_key_down_(this, event);
                return true;
            }
            return false;
        }

        virtual bool handle_key_up(const KeyboardEvent& event) {
            if (on_key_up_) {
                on_key_up_(this, event);
                return true;
            }
            return false;
        }

        // Hit testing
        virtual bool contains_point(const basic_point<float>& point) const noexcept {
            return point.x >= bounds_.x && point.x <= bounds_.x + bounds_.w &&
                   point.y >= bounds_.y && point.y <= bounds_.y + bounds_.h;
        }

        // Property setters
        void set_bounds(const basic_rect<float>& bounds) {
            if (bounds_ == bounds) return;
            bounds_ = bounds;
            set_flag(WidgetFlag::LayoutDirty, true);
            mark_dirty();
        }

        void set_position(const basic_point<float>& pos) {
            set_bounds(basic_rect<float>(pos, bounds_.get_size()));
        }

        void set_size(const basic_size<float>& size) {
            set_bounds(basic_rect<float>(bounds_.get_point(), size));
        }

        void set_visible(bool visible) {
            set_flag(WidgetFlag::Visible, visible);
            mark_dirty();
        }

        void set_enabled(bool enabled) {
            set_flag(WidgetFlag::Enabled, enabled);
            mark_dirty();
        }

        void set_style(const WidgetStyle& style) {
            style_ = style;
            set_flag(WidgetFlag::LayoutDirty, true);
            mark_dirty();
        }

        void set_id(const std::string& id) {
            id_ = id;
        }

        // Property getters
        const basic_rect<float>& get_bounds() const noexcept { return bounds_; }
        const basic_rect<float>& get_content_bounds() const noexcept { return content_bounds_; }
        const WidgetStyle& get_style() const noexcept { return style_; }
        WidgetStyle& get_style() noexcept { return style_; }
        const std::string& get_id() const noexcept { return id_; }
        Container* get_parent() const noexcept { return parent_; }

        bool is_visible() const noexcept { return has_flag(flags_, WidgetFlag::Visible); }
        bool is_enabled() const noexcept { return has_flag(flags_, WidgetFlag::Enabled); }
        bool is_focusable() const noexcept { return has_flag(flags_, WidgetFlag::Focusable); }
        bool is_focused() const noexcept { return has_flag(flags_, WidgetFlag::Focused); }
        bool is_hovered() const noexcept { return has_flag(flags_, WidgetFlag::Hovered); }
        bool is_pressed() const noexcept { return has_flag(flags_, WidgetFlag::Pressed); }
        bool is_dirty() const noexcept { return has_flag(flags_, WidgetFlag::Dirty); }
        bool needs_layout() const noexcept { return has_flag(flags_, WidgetFlag::LayoutDirty); }

        // Event callback setters
        void on_mouse_down(std::function<void(Widget*, const MouseEvent&)> callback) {
            on_mouse_down_ = std::move(callback);
        }

        void on_mouse_up(std::function<void(Widget*, const MouseEvent&)> callback) {
            on_mouse_up_ = std::move(callback);
        }

        void on_mouse_move(std::function<void(Widget*, const MouseEvent&)> callback) {
            on_mouse_move_ = std::move(callback);
        }

        void on_key_down(std::function<void(Widget*, const KeyboardEvent&)> callback) {
            on_key_down_ = std::move(callback);
        }

        void on_key_up(std::function<void(Widget*, const KeyboardEvent&)> callback) {
            on_key_up_ = std::move(callback);
        }

        // Focus management
        void set_focusable(bool focusable) {
            set_flag(WidgetFlag::Focusable, focusable);
        }

        void set_focused(bool focused) {
            if (is_focused() == focused) return;
            
            set_flag(WidgetFlag::Focused, focused);
            
            if (focused && on_focus_gained_) {
                on_focus_gained_(this);
            } else if (!focused && on_focus_lost_) {
                on_focus_lost_(this);
            }
            
            mark_dirty();
        }

        void set_hovered(bool hovered) {
            if (is_hovered() == hovered) return;
            set_flag(WidgetFlag::Hovered, hovered);
            mark_dirty();
        }

        void set_pressed(bool pressed) {
            if (is_pressed() == pressed) return;
            set_flag(WidgetFlag::Pressed, pressed);
            mark_dirty();
        }

        friend class Container;
    };

} // namespace zuu::widget