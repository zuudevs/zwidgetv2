#pragma once

#include "canvas.hpp"
#include "zwidget/unit/rect.hpp"
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <vector>
#include <Windows.h>

#if defined(max) && defined(min)
    #undef max
    #undef min
#endif

namespace zuu::widget {

    // Dirty region tracker untuk partial redraw
    class DirtyRegionTracker {
    private:
        std::vector<basic_rect<int>> dirty_regions_;
        bool is_full_dirty_{false};

    public:
        void mark_dirty(const basic_rect<int>& region) {
            if (is_full_dirty_) return;
			if (region.w <= 0 || region.h <= 0) return;

            // Merge overlapping regions untuk optimisasi
            bool merged = false;
            for (auto& existing : dirty_regions_) {
                if (regions_overlap(existing, region)) {
                    existing = merge_regions(existing, region);
                    merged = true;
                    break;
                }
            }

            if (!merged) {
                dirty_regions_.push_back(region);
            }

            // Jika terlalu banyak region, mark sebagai full dirty
            if (dirty_regions_.size() > 10) {
                is_full_dirty_ = true;
                dirty_regions_.clear();
            }
        }

        void mark_full_dirty() {
            is_full_dirty_ = true;
            dirty_regions_.clear();
        }

        void clear() {
            dirty_regions_.clear();
            is_full_dirty_ = false;
        }

        bool is_dirty() const noexcept {
            return is_full_dirty_ || !dirty_regions_.empty();
        }

        bool is_full_dirty() const noexcept {
            return is_full_dirty_;
        }

        const std::vector<basic_rect<int>>& get_regions() const noexcept {
            return dirty_regions_;
        }

    private:
        static bool regions_overlap(const basic_rect<int>& a, const basic_rect<int>& b) {
            return !(a.x + a.w < b.x || b.x + b.w < a.x ||
                     a.y + a.h < b.y || b.y + b.h < a.y);
        }

        static basic_rect<int> merge_regions(const basic_rect<int>& a, const basic_rect<int>& b) {
            int x1 = (a.x < b.x) ? a.x : b.x;
            int y1 = (a.y < b.y) ? a.y : b.y;
            int x2 = ((a.x + a.w) > (b.x + b.w)) ? (a.x + a.w) : (b.x + b.w);
            int y2 = ((a.y + a.h) > (b.y + b.h)) ? (a.y + a.h) : (b.y + b.h);
            return basic_rect<int>(x1, y1, x2 - x1, y2 - y1);
        }
    };

    // Renderer class - mengelola Direct2D resources
    class Renderer : public Canvas {
    private:
        static inline Microsoft::WRL::ComPtr<ID2D1Factory> d2d_factory_;
        static inline Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
        static inline bool factories_initialized_{false};

        Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> hwnd_render_target_;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> default_text_format_;
        
        HWND hwnd_{nullptr};
        DirtyRegionTracker dirty_tracker_;
        bool in_draw_{false};

    public:
        Renderer() = default;

        ~Renderer() {
            cleanup();
        }

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&& other) noexcept
            : Canvas(std::move(other))
            , hwnd_render_target_(std::move(other.hwnd_render_target_))
            , default_text_format_(std::move(other.default_text_format_))
            , hwnd_(std::exchange(other.hwnd_, nullptr))
            , dirty_tracker_(std::move(other.dirty_tracker_))
            , in_draw_(other.in_draw_) {}

        Renderer& operator=(Renderer&& other) noexcept {
            if (this != &other) {
                cleanup();
                Canvas::operator=(std::move(other));
                hwnd_render_target_ = std::move(other.hwnd_render_target_);
                default_text_format_ = std::move(other.default_text_format_);
                hwnd_ = std::exchange(other.hwnd_, nullptr);
                dirty_tracker_ = std::move(other.dirty_tracker_);
                in_draw_ = other.in_draw_;
            }
            return *this;
        }

        // Initialize factories (call once at startup)
        static bool initialize_factories() {
            if (factories_initialized_) return true;

            HRESULT hr = D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED,
                d2d_factory_.GetAddressOf()
            );

            if (FAILED(hr)) return false;

            hr = DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(dwrite_factory_.GetAddressOf())
            );

            if (FAILED(hr)) return false;

            factories_initialized_ = true;
            return true;
        }

        // Initialize renderer for a window
        bool initialize(HWND hwnd, const basic_size<int>& size) {
            if (!factories_initialized_) {
                if (!initialize_factories()) {
                    return false;
                }
            }

            hwnd_ = hwnd;

            // Create render target
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                0, 0,
                D2D1_RENDER_TARGET_USAGE_NONE,
                D2D1_FEATURE_LEVEL_DEFAULT
            );

            D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_props = D2D1::HwndRenderTargetProperties(
                hwnd,
                D2D1::SizeU(size.w, size.h)
            );

            HRESULT hr = d2d_factory_->CreateHwndRenderTarget(
                props,
                hwnd_props,
                hwnd_render_target_.GetAddressOf()
            );

            if (FAILED(hr)) return false;

            render_target_ = hwnd_render_target_;

            // Create brush
            hr = render_target_->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                brush_.GetAddressOf()
            );

            if (FAILED(hr)) return false;

            // Create default text format
            hr = dwrite_factory_->CreateTextFormat(
                L"Segoe UI",
                nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                14.0f,
                L"en-us",
                default_text_format_.GetAddressOf()
            );

            if (FAILED(hr)) return false;

            dirty_tracker_.mark_full_dirty();
            return true;
        }

        // Resize render target
        void resize(const basic_size<int>& new_size) {
            if (hwnd_render_target_) {
                hwnd_render_target_->Resize(D2D1::SizeU(new_size.w, new_size.h));
                dirty_tracker_.mark_full_dirty();
            }
        }

        // Dirty region management
        void invalidate(const basic_rect<int>& region) {
            dirty_tracker_.mark_dirty(region);
            if (hwnd_) {
                RECT rect = { region.x, region.y, region.x + region.w, region.y + region.h };
                InvalidateRect(hwnd_, &rect, FALSE);
            }
        }

        void invalidate_full() {
            dirty_tracker_.mark_full_dirty();
            if (hwnd_) {
                InvalidateRect(hwnd_, nullptr, FALSE);
            }
        }

        bool needs_redraw() const noexcept {
            return dirty_tracker_.is_dirty();
        }

        // Begin/End draw cycle
        bool begin_draw() {
            if (!render_target_ || in_draw_) return false;

            render_target_->BeginDraw();
            in_draw_ = true;
            return true;
        }

        bool end_draw() {
            if (!render_target_ || !in_draw_) return false;

            HRESULT hr = render_target_->EndDraw();
            in_draw_ = false;
            
            if (SUCCEEDED(hr)) {
                dirty_tracker_.clear();
                return true;
            }

            // Device lost - need to recreate
            if (hr == D2DERR_RECREATE_TARGET) {
                cleanup_device_resources();
                return false;
            }

            return false;
        }

        bool recreate_device_resources(const basic_size<int>& size) {
			cleanup_device_resources();
			return initialize(hwnd_, size);
		}

		// Modify render function
		template<typename DrawFunc>
		bool render(DrawFunc&& draw_func) {
			if (!needs_redraw()) return true;

			if (!begin_draw()) return false;

			if (dirty_tracker_.is_full_dirty()) {
				draw_func(*this);
			} else {
				for (const auto& region : dirty_tracker_.get_regions()) {
					push_clip(basic_rect<float>(
						static_cast<float>(region.x),
						static_cast<float>(region.y),
						static_cast<float>(region.w),
						static_cast<float>(region.h)
					));
					
					draw_func(*this);
					
					pop_clip();
				}
			}

			bool success = end_draw();
			
			// If device was lost, try to recreate
			if (!success && hwnd_) {
				RECT rect;
				GetClientRect(hwnd_, &rect);
				basic_size<int> size(rect.right - rect.left, rect.bottom - rect.top);
				
				if (recreate_device_resources(size)) {
					dirty_tracker_.mark_full_dirty();
					return false; // Needs another render pass
				}
			}
			
			return success;
		}

        // Text format management
        IDWriteTextFormat* get_default_text_format() const noexcept {
            return default_text_format_.Get();
        }

        Microsoft::WRL::ComPtr<IDWriteTextFormat> create_text_format(
            const std::wstring& font_family,
            float font_size,
            DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL
        ) {
            Microsoft::WRL::ComPtr<IDWriteTextFormat> format;
            
            HRESULT hr = dwrite_factory_->CreateTextFormat(
                font_family.c_str(),
                nullptr,
                weight,
                style,
                DWRITE_FONT_STRETCH_NORMAL,
                font_size,
                L"en-us",
                format.GetAddressOf()
            );

            return SUCCEEDED(hr) ? format : nullptr;
        }

        // Override draw_text untuk menggunakan default text format
        void draw_text(
            const std::wstring& text,
            const basic_rect<float>& rect,
            const Color& color
        ) {
            Canvas::draw_text(text, rect, color, default_text_format_.Get());
        }

        // Getters
        static ID2D1Factory* get_d2d_factory() noexcept {
            return d2d_factory_.Get();
        }

        static IDWriteFactory* get_dwrite_factory() noexcept {
            return dwrite_factory_.Get();
        }

        bool is_initialized() const noexcept {
            return hwnd_render_target_ != nullptr && render_target_ != nullptr && brush_ != nullptr;
        }

    private:
        void cleanup_device_resources() {
            brush_.Reset();
            render_target_.Reset();
            hwnd_render_target_.Reset();
        }

        void cleanup() {
            cleanup_device_resources();
            default_text_format_.Reset();
            hwnd_ = nullptr;
            in_draw_ = false;
        }
    };

} // namespace zuu::widget