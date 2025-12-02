#pragma once

#include "widget.hpp"
#include "zwidget/unit/align.hpp"

namespace zuu::widget {

    class Label : public Widget {
    private:
        std::wstring text_;
        QAlign h_align_{QAlign::start};
        QAlign v_align_{QAlign::center};
        bool word_wrap_{false};

    public:
        Label() {
            style_.background_color = Color::Transparent();
            style_.text_color = Color::White();
        }

        explicit Label(const std::wstring& text) : Label() {
            text_ = text;
        }

        void render(Renderer& renderer) override {
            if (!is_visible()) return;

            // Draw background if not transparent
            if (style_.background_color.a() > 0) {
                Widget::render(renderer);
            }

            // Draw text
            if (!text_.empty()) {
                // TODO: Implement proper text alignment
                // For now, just draw in content bounds
                renderer.draw_text(text_, content_bounds_, style_.text_color);
            }

            set_flag(WidgetFlag::Dirty, false);
        }

        // Setters
        void set_text(const std::wstring& text) {
            if (text_ != text) {
                text_ = text;
                mark_dirty();
            }
        }

        void set_horizontal_alignment(QAlign align) {
            if (h_align_ != align) {
                h_align_ = align;
                mark_dirty();
            }
        }

        void set_vertical_alignment(QAlign align) {
            if (v_align_ != align) {
                v_align_ = align;
                mark_dirty();
            }
        }

        void set_word_wrap(bool wrap) {
            if (word_wrap_ != wrap) {
                word_wrap_ = wrap;
                set_flag(WidgetFlag::LayoutDirty, true);
                mark_dirty();
            }
        }

        void set_text_color(const Color& color) {
            style_.text_color = color;
            mark_dirty();
        }

        // Getters
        const std::wstring& get_text() const noexcept {
            return text_;
        }

        QAlign get_horizontal_alignment() const noexcept {
            return h_align_;
        }

        QAlign get_vertical_alignment() const noexcept {
            return v_align_;
        }

        bool get_word_wrap() const noexcept {
            return word_wrap_;
        }
    };

} // namespace zuu::widget