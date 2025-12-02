#include "zwidget/unit/window.hpp"
#include "zwidget/core/application.hpp"
#include "zwidget/graphic/renderer.hpp"
#include <print>
#include <vector>
#include <random>
#include <chrono>

using namespace zuu::widget;

// Helper untuk generate random color
Color random_color() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return Color(dis(gen), dis(gen), dis(gen));
}

// Demo: Partial redraw dengan animated rectangles
class AnimatedRectDemo {
private:
    struct AnimatedRect {
        basic_rect<float> rect;
        Color color;
        basic_point<float> velocity;
        
        void update(float dt, const basic_size<int>& bounds) {
            rect.x += velocity.x * dt;
            rect.y += velocity.y * dt;
            
            // Bounce off edges
            if (rect.x < 0 || rect.x + rect.w > bounds.w) {
                velocity.x = -velocity.x;
                rect.x = std::clamp(rect.x, 0.0f, bounds.w - rect.w);
            }
            if (rect.y < 0 || rect.y + rect.h > bounds.h) {
                velocity.y = -velocity.y;
                rect.y = std::clamp(rect.y, 0.0f, bounds.h - rect.h);
            }
        }
        
        basic_rect<int> get_invalidation_rect() const {
            // Expand by velocity untuk cover motion
            float expand = std::max(std::abs(velocity.x), std::abs(velocity.y)) * 2.0f;
            return basic_rect<int>(
                static_cast<int>(rect.x - expand),
                static_cast<int>(rect.y - expand),
                static_cast<int>(rect.w + expand * 2),
                static_cast<int>(rect.h + expand * 2)
            );
        }
    };
    
    std::vector<AnimatedRect> rects_;
    basic_size<int> window_size_;
    std::chrono::steady_clock::time_point last_update_;
    bool show_info_{true};
    
public:
    AnimatedRectDemo(const basic_size<int>& window_size) 
        : window_size_(window_size)
        , last_update_(std::chrono::steady_clock::now()) {
        
        // Create random animated rectangles
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> pos_x(0, window_size.w - 100.0f);
        std::uniform_real_distribution<float> pos_y(0, window_size.h - 100.0f);
        std::uniform_real_distribution<float> size_dist(30.0f, 80.0f);
        std::uniform_real_distribution<float> vel_dist(-100.0f, 100.0f);
        
        for (int i = 0; i < 5; ++i) {
            AnimatedRect ar;
            ar.rect = basic_rect<float>(
                pos_x(gen), pos_y(gen),
                size_dist(gen), size_dist(gen)
            );
            ar.color = random_color();
            ar.velocity = basic_point<float>(vel_dist(gen), vel_dist(gen));
            rects_.push_back(ar);
        }
    }
    
    void update(Window& window) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last_update_).count();
        last_update_ = now;
        
        // Update rectangles and invalidate their regions
        for (auto& rect : rects_) {
            auto old_region = rect.get_invalidation_rect();
            rect.update(dt, window_size_);
            auto new_region = rect.get_invalidation_rect();
            
            // Invalidate both old and new positions
            window.invalidate(old_region);
            window.invalidate(new_region);
        }
        
        // Invalidate info region if visible
        if (show_info_) {
            window.invalidate(basic_rect<int>(10, 10, 250, 80));
        }
    }
    
    void render(Renderer& renderer) {
        // Clear background
        renderer.clear(Color::from_hex(0x1e1e1e));
        
        // Draw animated rectangles
        for (const auto& rect : rects_) {
            renderer.fill_rounded_rect(rect.rect, 10.0f, 10.0f, rect.color);
            
            // Draw outline
            Color outline = Color(
                rect.color.r() * 0.7f,
                rect.color.g() * 0.7f,
                rect.color.b() * 0.7f
            );
            renderer.draw_rounded_rect(rect.rect, 10.0f, 10.0f, outline, 2.0f);
        }
        
        // Draw info panel
        if (show_info_) {
            auto info_rect = basic_rect<float>(10, 10, 240, 70);
            renderer.fill_rounded_rect(info_rect, 8.0f, 8.0f, Color(0.1f, 0.1f, 0.1f, 0.8f));
            renderer.draw_rounded_rect(info_rect, 8.0f, 8.0f, Color::White(), 1.0f);
            
            // Draw text
            renderer.draw_text(
                L"Partial Redraw Demo",
                basic_rect<float>(20, 20, 220, 25),
                Color::White()
            );
            renderer.draw_text(
                L"Press SPACE to toggle info",
                basic_rect<float>(20, 45, 220, 25),
                Color::LightGray()
            );
        }
    }
    
    void toggle_info() {
        show_info_ = !show_info_;
    }
    
    void resize(const basic_size<int>& new_size) {
        window_size_ = new_size;
    }
};

// Demo: Interactive drawing
class InteractiveDrawDemo {
private:
    struct DrawnShape {
        basic_point<float> start;
        basic_point<float> end;
        Color color;
        float thickness;
    };
    
    std::vector<DrawnShape> shapes_;
    basic_point<float> current_start_;
    basic_point<float> current_end_;
    bool is_drawing_{false};
    Color current_color_{Color::White()};
    float current_thickness_{2.0f};
    
public:
    void on_mouse_down(const basic_point<uint16_t>& pos) {
        is_drawing_ = true;
        current_start_ = basic_point<float>(
            static_cast<float>(pos.x),
            static_cast<float>(pos.y)
        );
        current_end_ = current_start_;
    }
    
    void on_mouse_move(const basic_point<uint16_t>& pos, Window& window) {
        if (!is_drawing_) return;
        
        auto old_end = current_end_;
        current_end_ = basic_point<float>(
            static_cast<float>(pos.x),
            static_cast<float>(pos.y)
        );
        
        // Invalidate the area between start and both end points
        float min_x = std::min({current_start_.x, old_end.x, current_end_.x});
        float max_x = std::max({current_start_.x, old_end.x, current_end_.x});
        float min_y = std::min({current_start_.y, old_end.y, current_end_.y});
        float max_y = std::max({current_start_.y, old_end.y, current_end_.y});
        
        float padding = current_thickness_ * 2;
        window.invalidate(basic_rect<int>(
            static_cast<int>(min_x - padding),
            static_cast<int>(min_y - padding),
            static_cast<int>(max_x - min_x + padding * 2),
            static_cast<int>(max_y - min_y + padding * 2)
        ));
    }
    
    void on_mouse_up(const basic_point<uint16_t>& pos, Window& window) {
        if (!is_drawing_) return;
        
        current_end_ = basic_point<float>(
            static_cast<float>(pos.x),
            static_cast<float>(pos.y)
        );
        
        shapes_.push_back({
            current_start_,
            current_end_,
            current_color_,
            current_thickness_
        });
        
        is_drawing_ = false;
        window.invalidate();
    }
    
    void render(Renderer& renderer) {
        // Clear background
        renderer.clear(Color::from_hex(0x2b2b2b));
        
        // Draw all completed shapes
        for (const auto& shape : shapes_) {
            renderer.draw_line(shape.start, shape.end, shape.color, shape.thickness);
        }
        
        // Draw current shape being drawn
        if (is_drawing_) {
            renderer.draw_line(current_start_, current_end_, current_color_, current_thickness_);
        }
        
        // Draw toolbar
        auto toolbar_rect = basic_rect<float>(10, 10, 180, 50);
        renderer.fill_rounded_rect(toolbar_rect, 8.0f, 8.0f, Color(0.15f, 0.15f, 0.15f, 0.9f));
        renderer.draw_text(
            L"Click and drag to draw",
            basic_rect<float>(20, 20, 160, 30),
            Color::White()
        );
    }
    
    void change_color() {
        current_color_ = random_color();
    }
    
    void clear() {
        shapes_.clear();
    }
};

int main() {
    try {
        std::println("=== ZWidget Rendering Demo ===\n");
        
        // Initialize D2D factories
        if (!Renderer::initialize_factories()) {
            std::println("❌ Failed to initialize D2D factories");
            return 1;
        }
        std::println("✅ Direct2D initialized\n");
        
        // Initialize Application
        if (!Application::initialize("ZWidgetRenderingDemo")) {
            std::println("❌ Failed to initialize application");
            return 1;
        }
        
        // Demo 1: Animated rectangles with partial redraw
        Window anim_window("Animated Rectangles - Partial Redraw", Size(800, 600));
        AnimatedRectDemo anim_demo(anim_window.get_size());
        
        anim_window.set_paint_callback([&](Renderer& r) {
            anim_demo.render(r);
        });
        
        anim_window.show();
        std::println("✅ Created animated demo window\n");
        
        // Demo 2: Interactive drawing
        Window draw_window("Interactive Drawing", Point(850, 100), Size(800, 600));
        InteractiveDrawDemo draw_demo;
        
        draw_window.set_paint_callback([&](Renderer& r) {
            draw_demo.render(r);
        });
        
        draw_window.show();
        std::println("✅ Created interactive drawing window\n");
        
        std::println("📝 Controls:");
        std::println("   [Animated Window]");
        std::println("   - SPACE: Toggle info panel");
        std::println("   [Drawing Window]");
        std::println("   - Click and drag: Draw lines");
        std::println("   - C: Change color");
        std::println("   - X: Clear canvas");
        std::println("   - ESC: Close window\n");
        
        // Event loop
        auto last_frame = std::chrono::steady_clock::now();
        
        while (Application::is_running()) {
            Event event;
            
            while (EventDispatcher::PollEvent(event)) {
                if (event.is_quit_event()) {
                    break;
                }
                
                Window* event_window = event.get_window();
                
                // Handle animated window events
                if (event_window == &anim_window) {
                    if (auto* ke = event.get_if<KeyboardEvent>()) {
                        if (ke->get_type() == KeyboardEvent::Type::key_press) {
                            if (ke->get_key() == KeyboardEvent::KeyCode::Space) {
                                anim_demo.toggle_info();
                                anim_window.invalidate();
                            } else if (ke->get_key() == KeyboardEvent::KeyCode::Escape) {
                                anim_window.close();
                            }
                        }
                    } else if (auto* we = event.get_if<WindowEvent>()) {
                        if (we->get_type() == WindowEvent::Type::resize) {
                            anim_demo.resize(Size(we->get_size().w, we->get_size().h));
                        }
                    }
                }
                
                // Handle drawing window events
                if (event_window == &draw_window) {
                    if (auto* me = event.get_if<MouseEvent>()) {
                        if (me->get_type() == MouseEvent::Type::button_press &&
                            me->get_button() == MouseEvent::Button::left) {
                            draw_demo.on_mouse_down(me->get_position());
                        } else if (me->get_type() == MouseEvent::Type::button_release &&
                                   me->get_button() == MouseEvent::Button::left) {
                            draw_demo.on_mouse_up(me->get_position(), draw_window);
                        } else if (me->get_type() == MouseEvent::Type::move) {
                            draw_demo.on_mouse_move(me->get_position(), draw_window);
                        }
                    } else if (auto* ke = event.get_if<KeyboardEvent>()) {
                        if (ke->get_type() == KeyboardEvent::Type::key_press) {
                            if (ke->get_key() == KeyboardEvent::KeyCode::C) {
                                draw_demo.change_color();
                            } else if (ke->get_key() == KeyboardEvent::KeyCode::X) {
                                draw_demo.clear();
                                draw_window.invalidate();
                            } else if (ke->get_key() == KeyboardEvent::KeyCode::Escape) {
                                draw_window.close();
                            }
                        }
                    }
                }
            }
            
            // Update animation (60 FPS target)
            auto now = std::chrono::steady_clock::now();
            auto dt = std::chrono::duration<float>(now - last_frame).count();
            
            if (dt >= 1.0f / 60.0f) {
                if (anim_window.is_valid()) {
                    anim_demo.update(anim_window);
                }
                last_frame = now;
            }
            
            // Small sleep to reduce CPU usage
            Sleep(1);
        }
        
        std::println("\n=== Demo Exited Successfully ===");
        return 0;
        
    } catch (const std::exception& e) {
        std::println("❌ Exception: {}", e.what());
        return 1;
    }
}