#pragma once

#include "color.hpp"
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

#ifdef min
	#undef min
#endif
#ifdef max
	#undef max
#endif

#include "zwidget/unit/rect.hpp"
#include <string>

namespace zuu::widget {

    // Canvas adalah abstraksi untuk drawing operations
    class Canvas {
    protected:
        Microsoft::WRL::ComPtr<ID2D1RenderTarget> render_target_;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush_;

    public:
        Canvas() = default;
        virtual ~Canvas() = default;

        Canvas(const Canvas&) = delete;
        Canvas& operator=(const Canvas&) = delete;
        Canvas(Canvas&&) = default;
        Canvas& operator=(Canvas&&) = default;

        // Basic drawing operations
        virtual void clear(const Color& color) {
            if (render_target_) {
                render_target_->Clear(color.to_d2d());
            }
        }

        virtual void draw_line(
            const basic_point<float>& start,
            const basic_point<float>& end,
            const Color& color,
            float width = 1.0f
        ) {
            if (!render_target_ || !brush_) return;

            brush_->SetColor(color.to_d2d());
            render_target_->DrawLine(
                D2D1::Point2F(start.x, start.y),
                D2D1::Point2F(end.x, end.y),
                brush_.Get(),
                width
            );
        }

        virtual void draw_rect(
            const basic_rect<float>& rect,
            const Color& color,
            float width = 1.0f
        ) {
            if (!render_target_ || !brush_) return;

            brush_->SetColor(color.to_d2d());
            render_target_->DrawRectangle(
                D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
                brush_.Get(),
                width
            );
        }

        virtual void fill_rect(
            const basic_rect<float>& rect,
            const Color& color
        ) {
            if (!render_target_ || !brush_) return;

            brush_->SetColor(color.to_d2d());
            render_target_->FillRectangle(
                D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
                brush_.Get()
            );
        }

        virtual void draw_rounded_rect(
            const basic_rect<float>& rect,
            float radius_x,
            float radius_y,
            const Color& color,
            float width = 1.0f
        ) {
            if (!render_target_ || !brush_) return;

            brush_->SetColor(color.to_d2d());
            render_target_->DrawRoundedRectangle(
                D2D1::RoundedRect(
                    D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
                    radius_x,
                    radius_y
                ),
                brush_.Get(),
                width
            );
        }

        virtual void fill_rounded_rect(
            const basic_rect<float>& rect,
            float radius_x,
            float radius_y,
            const Color& color
        ) {
            if (!render_target_ || !brush_) return;

            brush_->SetColor(color.to_d2d());
            render_target_->FillRoundedRectangle(
                D2D1::RoundedRect(
                    D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
                    radius_x,
                    radius_y
                ),
                brush_.Get()
            );
        }

        virtual void draw_ellipse(
            const basic_point<float>& center,
            float radius_x,
            float radius_y,
            const Color& color,
            float width = 1.0f
        ) {
            if (!render_target_ || !brush_) return;

            brush_->SetColor(color.to_d2d());
            render_target_->DrawEllipse(
                D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius_x, radius_y),
                brush_.Get(),
                width
            );
        }

        virtual void fill_ellipse(
            const basic_point<float>& center,
            float radius_x,
            float radius_y,
            const Color& color
        ) {
            if (!render_target_ || !brush_) return;

            brush_->SetColor(color.to_d2d());
            render_target_->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius_x, radius_y),
                brush_.Get()
            );
        }

        virtual void draw_circle(
            const basic_point<float>& center,
            float radius,
            const Color& color,
            float width = 1.0f
        ) {
            draw_ellipse(center, radius, radius, color, width);
        }

        virtual void fill_circle(
            const basic_point<float>& center,
            float radius,
            const Color& color
        ) {
            fill_ellipse(center, radius, radius, color);
        }

        // Text rendering (simplified - can be extended)
        virtual void draw_text(
            const std::wstring& text,
            const basic_rect<float>& rect,
            const Color& color,
            IDWriteTextFormat* text_format = nullptr
        ) {
            if (!render_target_ || !brush_ || !text_format) return;

            brush_->SetColor(color.to_d2d());
            render_target_->DrawText(
                text.c_str(),
                static_cast<UINT32>(text.length()),
                text_format,
                D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
                brush_.Get()
            );
        }

        // Clipping
        virtual void push_clip(const basic_rect<float>& rect) {
            if (!render_target_) return;

            render_target_->PushAxisAlignedClip(
                D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
            );
        }

        virtual void pop_clip() {
            if (!render_target_) return;
            render_target_->PopAxisAlignedClip();
        }

        // Transform operations
        virtual void save() {
            // Can be extended for state saving
        }

        virtual void restore() {
            // Can be extended for state restoration
        }

        // Getters
        ID2D1RenderTarget* get_render_target() const noexcept {
            return render_target_.Get();
        }

        ID2D1SolidColorBrush* get_brush() const noexcept {
            return brush_.Get();
        }

        bool is_valid() const noexcept {
            return render_target_ != nullptr && brush_ != nullptr;
        }
    };

} // namespace zuu::widget