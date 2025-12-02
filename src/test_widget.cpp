#include "zwidget/unit/window.hpp"
#include "zwidget/core/application.hpp"
#include "zwidget/widgets/widget.hpp"
#include "zwidget/widgets/container.hpp"
#include "zwidget/widgets/button.hpp"
#include "zwidget/widgets/label.hpp"
#include "zwidget/widgets/panel.hpp"
#include <print>
#include <memory>

using namespace zuu::widget;

int main() {
    try {
        std::println("=== ZWidget Widget System Demo ===\n");

        // Initialize
        if (!Renderer::initialize_factories()) {
            std::println("❌ Failed to initialize D2D");
            return 1;
        }

        if (!Application::initialize("WidgetSystemDemo")) {
            std::println("❌ Failed to initialize application");
            return 1;
        }

        std::println("✅ Application initialized\n");

        // Create window
        Window window("Widget System Demo", Size(900, 700));

        // Create root container
        auto root = std::make_unique<Panel>();
        root->set_bounds(basic_rect<float>(0, 0, 900, 700));
        root->get_style().padding = {20.0f, 20.0f, 20.0f, 20.0f};

        // Create title label
        auto* title = root->add_child<Label>(L"Widget System Demo");
        title->set_bounds(basic_rect<float>(0, 0, 860, 40));
        title->set_text_color(Color::from_hex(0x4a90e2));

        // Create vertical stack for buttons
        auto* button_panel = root->add_child<StackPanel>(LayoutDirection::Vertical);
        button_panel->set_bounds(basic_rect<float>(0, 50, 860, 300));
        button_panel->set_spacing(10.0f);

        // Add buttons
        auto* btn1 = button_panel->add_child<Button>(L"Button 1");
        btn1->set_size(basic_size<float>(200, 40));
        btn1->on_click([](Button* btn) {
            std::println("✅ Button 1 clicked!");
            btn->set_text(L"Clicked!");
        });

        auto* btn2 = button_panel->add_child<Button>(L"Button 2");
        btn2->set_size(basic_size<float>(200, 40));
        btn2->set_colors(
            Color::from_hex(0x2ecc71),
            Color::from_hex(0x27ae60),
            Color::from_hex(0x229954),
            Color::from_hex(0x1e8449)
        );
        btn2->on_click([](Button* btn) {
            std::println("✅ Button 2 clicked!");
        });

        auto* btn3 = button_panel->add_child<Button>(L"Button 3");
        btn3->set_size(basic_size<float>(200, 40));
        btn3->set_colors(
            Color::from_hex(0xe74c3c),
            Color::from_hex(0xc0392b),
            Color::from_hex(0xa93226),
            Color::from_hex(0x922b21)
        );
        btn3->on_click([](Button* btn) {
            std::println("✅ Button 3 clicked!");
        });

        // Create grid panel
        auto* grid = root->add_child<GridPanel>(2, 3);
        grid->set_bounds(basic_rect<float>(0, 370, 860, 250));
        grid->set_spacing(10.0f, 10.0f);

        // Add grid items
        for (int i = 0; i < 6; ++i) {
            auto* panel = grid->add_child<Panel>();
            
            auto* label = panel->add_child<Label>(
                L"Grid " + std::to_wstring(i + 1)
            );
            label->set_bounds(basic_rect<float>(10, 10, 100, 30));
            
            float hue = (i * 60.0f) / 360.0f;
            panel->get_style().background_color = Color(
                0.5f + 0.5f * std::cos(hue * 6.28f),
                0.5f + 0.5f * std::cos((hue + 0.33f) * 6.28f),
                0.5f + 0.5f * std::cos((hue + 0.67f) * 6.28f),
                1.0f
            );
        }

        // Info label
        auto* info = root->add_child<Label>(
            L"Press TAB to cycle focus, SPACE/ENTER to activate buttons"
        );
        info->set_bounds(basic_rect<float>(0, 630, 860, 30));
        info->set_text_color(Color::Gray());

        // Perform initial layout
        root->layout();

        // Set paint callback
        window.set_paint_callback([&root](Renderer& r) {
            r.clear(Color::from_hex(0x1a1a1a));
            root->render(r);
        });

        window.show();
        std::println("✅ Window created and shown\n");
        std::println("📝 Controls:");
        std::println("   - Click buttons to test interaction");
        std::println("   - Press TAB to cycle focus");
        std::println("   - Press SPACE/ENTER on focused button to activate");
        std::println("   - Press ESC to exit\n");

        // Track focused widget
        Widget* focused_widget = nullptr;
        std::vector<Widget*> focusable_widgets;

        // Collect focusable widgets
        std::function<void(Container*)> collect_focusable;
        collect_focusable = [&](Container* container) {
            for (auto& child : container->get_children()) {
                if (child->is_focusable()) {
                    focusable_widgets.push_back(child.get());
                }
                if (auto* sub_container = dynamic_cast<Container*>(child.get())) {
                    collect_focusable(sub_container);
                }
            }
        };
        collect_focusable(root.get());

        if (!focusable_widgets.empty()) {
            focused_widget = focusable_widgets[0];
            focused_widget->set_focused(true);
        }

        // Event loop
        auto last_frame = std::chrono::steady_clock::now();

        while (Application::is_running()) {
            Event event;

            while (EventDispatcher::PollEvent(event)) {
                if (event.is_quit_event()) {
                    break;
                }

                if (auto* we = event.get_if<WindowEvent>()) {
                    if (we->get_type() == WindowEvent::Type::resize) {
                        auto new_size = Size(we->get_size().w, we->get_size().h);
                        root->set_size(basic_size<float>(
                            static_cast<float>(new_size.w),
                            static_cast<float>(new_size.h)
                        ));
                        root->layout();
                        window.invalidate();
                    }
                }

                if (auto* me = event.get_if<MouseEvent>()) {
                    auto pos = basic_point<float>(
                        static_cast<float>(me->get_position().x),
                        static_cast<float>(me->get_position().y)
                    );

                    if (me->get_type() == MouseEvent::Type::move) {
                        root->handle_mouse_move(*me);
                        window.invalidate();
                    }
                    else if (me->get_type() == MouseEvent::Type::button_press) {
                        if (root->handle_mouse_down(*me)) {
                            // Update focused widget
                            Widget* clicked = root->find_widget_at(pos);
                            if (clicked && clicked->is_focusable()) {
                                if (focused_widget) {
                                    focused_widget->set_focused(false);
                                }
                                focused_widget = clicked;
                                focused_widget->set_focused(true);
                            }
                            window.invalidate();
                        }
                    }
                    else if (me->get_type() == MouseEvent::Type::button_release) {
                        if (root->handle_mouse_up(*me)) {
                            window.invalidate();
                        }
                    }
                }

                if (auto* ke = event.get_if<KeyboardEvent>()) {
                    if (ke->get_type() == KeyboardEvent::Type::key_press) {
                        if (ke->get_key() == KeyboardEvent::KeyCode::Escape) {
                            window.close();
                        }
                        else if (ke->get_key() == KeyboardEvent::KeyCode::Tab) {
                            // Cycle focus
                            if (!focusable_widgets.empty()) {
                                auto it = std::find(
                                    focusable_widgets.begin(),
                                    focusable_widgets.end(),
                                    focused_widget
                                );

                                if (it != focusable_widgets.end()) {
                                    focused_widget->set_focused(false);
                                    ++it;
                                    if (it == focusable_widgets.end()) {
                                        it = focusable_widgets.begin();
                                    }
                                    focused_widget = *it;
                                    focused_widget->set_focused(true);
                                    window.invalidate();
                                }
                            }
                        }
                        else if (focused_widget) {
                            if (focused_widget->handle_key_down(*ke)) {
                                window.invalidate();
                            }
                        }
                    }
                    else if (ke->get_type() == KeyboardEvent::Type::key_release) {
                        if (focused_widget) {
                            if (focused_widget->handle_key_up(*ke)) {
                                window.invalidate();
                            }
                        }
                    }
                }
            }

            // Update (for animations)
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - last_frame).count();

            if (dt >= 1.0f / 60.0f) {
                root->update(dt);
                last_frame = now;
            }

            Sleep(1);
        }

        std::println("\n=== Demo Exited Successfully ===");
        return 0;

    } catch (const std::exception& e) {
        std::println("❌ Exception: {}", e.what());
        return 1;
    }
}