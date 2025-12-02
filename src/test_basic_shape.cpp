#include "zwidget/unit/window.hpp"
#include "zwidget/core/application.hpp"
#include "zwidget/graphic/renderer.hpp"
#include <print>

using namespace zuu::widget;

int main() {
    try {
        std::println("=== ZWidget Basic Shapes Demo ===\n");
        
        // Initialize D2D
        if (!Renderer::initialize_factories()) {
            std::println("❌ Failed to initialize D2D");
            return 1;
        }
        
        // Initialize Application
        if (!Application::initialize("BasicShapesDemo")) {
            std::println("❌ Failed to initialize application");
            return 1;
        }
        
        std::println("✅ Application initialized\n");
        
        // Create window
        Window window("Basic Shapes - Direct2D Demo", Size(900, 700));
        
        // Set paint callback
        window.set_paint_callback([](Renderer& r) {
            // Clear with dark background
            r.clear(Color::from_hex(0x1a1a1a));
            
            float x_offset = 50.0f;
            float y_offset = 50.0f;
            float spacing = 150.0f;
            
            // Title
            r.draw_text(
                L"Direct2D Basic Shapes Gallery",
                basic_rect<float>(50, 10, 800, 30),
                Color::White()
            );
            
            // Row 1: Rectangles
            r.draw_text(
                L"Rectangles",
                basic_rect<float>(x_offset, y_offset, 120, 20),
                Color::LightGray()
            );
            
            // Filled rectangle
            r.fill_rect(
                basic_rect<float>(x_offset, y_offset + 30, 100, 80),
                Color::Red()
            );
            r.draw_text(
                L"Filled",
                basic_rect<float>(x_offset, y_offset + 115, 100, 20),
                Color::Gray()
            );
            
            // Outlined rectangle
            r.draw_rect(
                basic_rect<float>(x_offset + spacing, y_offset + 30, 100, 80),
                Color::Green(),
                3.0f
            );
            r.draw_text(
                L"Outlined",
                basic_rect<float>(x_offset + spacing, y_offset + 115, 100, 20),
                Color::Gray()
            );
            
            // Rounded rectangle
            r.fill_rounded_rect(
                basic_rect<float>(x_offset + spacing * 2, y_offset + 30, 100, 80),
                15.0f, 15.0f,
                Color::Blue()
            );
            r.draw_text(
                L"Rounded",
                basic_rect<float>(x_offset + spacing * 2, y_offset + 115, 100, 20),
                Color::Gray()
            );
            
            // Row 2: Circles
            y_offset += 180;
            r.draw_text(
                L"Circles & Ellipses",
                basic_rect<float>(x_offset, y_offset, 200, 20),
                Color::LightGray()
            );
            
            // Filled circle
            r.fill_circle(
                basic_point<float>(x_offset + 50, y_offset + 70),
                40.0f,
                Color::Yellow()
            );
            r.draw_text(
                L"Circle",
                basic_rect<float>(x_offset, y_offset + 115, 100, 20),
                Color::Gray()
            );
            
            // Outlined circle
            r.draw_circle(
                basic_point<float>(x_offset + spacing + 50, y_offset + 70),
                40.0f,
                Color::Cyan(),
                3.0f
            );
            r.draw_text(
                L"Outlined",
                basic_rect<float>(x_offset + spacing, y_offset + 115, 100, 20),
                Color::Gray()
            );
            
            // Ellipse
            r.fill_ellipse(
                basic_point<float>(x_offset + spacing * 2 + 50, y_offset + 70),
                50.0f, 30.0f,
                Color::Magenta()
            );
            r.draw_text(
                L"Ellipse",
                basic_rect<float>(x_offset + spacing * 2, y_offset + 115, 100, 20),
                Color::Gray()
            );
            
            // Row 3: Lines
            y_offset += 180;
            r.draw_text(
                L"Lines",
                basic_rect<float>(x_offset, y_offset, 100, 20),
                Color::LightGray()
            );
            
            // Thin line
            r.draw_line(
                basic_point<float>(x_offset, y_offset + 40),
                basic_point<float>(x_offset + 100, y_offset + 90),
                Color::White(),
                1.0f
            );
            r.draw_text(
                L"Thin",
                basic_rect<float>(x_offset, y_offset + 95, 100, 20),
                Color::Gray()
            );
            
            // Thick line
            r.draw_line(
                basic_point<float>(x_offset + spacing, y_offset + 40),
                basic_point<float>(x_offset + spacing + 100, y_offset + 90),
                Color::from_hex(0xff8800),
                5.0f
            );
            r.draw_text(
                L"Thick",
                basic_rect<float>(x_offset + spacing, y_offset + 95, 100, 20),
                Color::Gray()
            );
            
            // Multiple lines (star pattern)
            float center_x = x_offset + spacing * 2 + 50;
            float center_y = y_offset + 65;
            float radius = 40.0f;
            
            for (int i = 0; i < 8; ++i) {
                float angle = (i * 45.0f) * 3.14159f / 180.0f;
                float end_x = center_x + radius * std::cos(angle);
                float end_y = center_y + radius * std::sin(angle);
                
                r.draw_line(
                    basic_point<float>(center_x, center_y),
                    basic_point<float>(end_x, end_y),
                    Color(1.0f, 1.0f - i * 0.1f, i * 0.1f),
                    2.0f
                );
            }
            r.draw_text(
                L"Pattern",
                basic_rect<float>(x_offset + spacing * 2, y_offset + 95, 100, 20),
                Color::Gray()
            );
            
            // Row 4: Complex shapes
            y_offset += 180;
            r.draw_text(
                L"Complex Compositions",
                basic_rect<float>(x_offset, y_offset, 200, 20),
                Color::LightGray()
            );
            
            // Traffic light
            float tl_x = x_offset + 25;
            float tl_y = y_offset + 30;
            r.fill_rounded_rect(
                basic_rect<float>(tl_x, tl_y, 50, 120),
                10.0f, 10.0f,
                Color::DarkGray()
            );
            r.fill_circle(basic_point<float>(tl_x + 25, tl_y + 20), 15.0f, Color::Red());
            r.fill_circle(basic_point<float>(tl_x + 25, tl_y + 60), 15.0f, Color::Yellow());
            r.fill_circle(basic_point<float>(tl_x + 25, tl_y + 100), 15.0f, Color::Green());
            r.draw_text(
                L"Traffic Light",
                basic_rect<float>(x_offset, y_offset + 155, 100, 20),
                Color::Gray()
            );
            
            // Smiley face
            float smile_x = x_offset + spacing + 50;
            float smile_y = y_offset + 70;
            r.fill_circle(basic_point<float>(smile_x, smile_y), 50.0f, Color::Yellow());
            r.draw_circle(basic_point<float>(smile_x, smile_y), 50.0f, Color::from_hex(0xffaa00), 2.0f);
            // Eyes
            r.fill_circle(basic_point<float>(smile_x - 20, smile_y - 10), 5.0f, Color::Black());
            r.fill_circle(basic_point<float>(smile_x + 20, smile_y - 10), 5.0f, Color::Black());
            // Smile
            r.draw_line(
                basic_point<float>(smile_x - 20, smile_y + 15),
                basic_point<float>(smile_x + 20, smile_y + 15),
                Color::Black(),
                3.0f
            );
            r.draw_text(
                L"Smiley",
                basic_rect<float>(x_offset + spacing, y_offset + 155, 100, 20),
                Color::Gray()
            );
            
            // Color gradient simulation (multiple rectangles)
            float grad_x = x_offset + spacing * 2;
            float grad_y = y_offset + 30;
            for (int i = 0; i < 10; ++i) {
                float t = i / 9.0f;
                Color c = Color(t, 1.0f - t, 0.5f);
                r.fill_rect(
                    basic_rect<float>(grad_x + i * 10, grad_y, 10, 100),
                    c
                );
            }
            r.draw_rect(
                basic_rect<float>(grad_x, grad_y, 100, 100),
                Color::White(),
                1.0f
            );
            r.draw_text(
                L"Gradient",
                basic_rect<float>(x_offset + spacing * 2, y_offset + 155, 100, 20),
                Color::Gray()
            );
            
            // Footer
            r.draw_text(
                L"Press ESC to close",
                basic_rect<float>(50, 650, 800, 20),
                Color::Gray()
            );
        });
        
        window.show();
        std::println("✅ Window created and shown\n");
        std::println("📝 Press ESC to close the window\n");
        
        // Event loop
        while (Application::is_running()) {
            Event event;
            
            while (EventDispatcher::PollEvent(event)) {
                if (event.is_quit_event()) {
                    break;
                }
                
                if (auto* ke = event.get_if<KeyboardEvent>()) {
                    if (ke->get_type() == KeyboardEvent::Type::key_press &&
                        ke->get_key() == KeyboardEvent::KeyCode::Escape) {
                        window.close();
                    }
                }
            }
            
            Sleep(1);
        }
        
        std::println("=== Demo Exited Successfully ===");
        return 0;
        
    } catch (const std::exception& e) {
        std::println("❌ Exception: {}", e.what());
        return 1;
    }
}