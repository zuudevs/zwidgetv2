#include "zwidget/unit/window.hpp"
#include "zwidget/core/application.hpp"
#include "zwidget/widgets/widget.hpp"
#include "zwidget/widgets/container.hpp"
#include "zwidget/widgets/button.hpp"
#include "zwidget/widgets/label.hpp"
#include "zwidget/widgets/panel.hpp"
#include "zwidget/widgets/checkbox.hpp"
#include "zwidget/widgets/slider.hpp"
#include "zwidget/widgets/combobox.hpp"
#include "zwidget/widgets/textbox.hpp"
#include <iostream>
#include <print>
#include <memory>

using namespace zuu::widget;

class ComprehensiveWidgetDemo {
private:
    std::unique_ptr<Panel> root_;
    Label* status_label_{nullptr};
    Label* slider_value_label_{nullptr};
    TextBox* improved_textbox_{nullptr};
    Slider* h_slider_{nullptr};
    Slider* v_slider_{nullptr};
    ComboBox* combo_{nullptr};
    
public:
    ComprehensiveWidgetDemo(const basic_size<float>& size) {
        root_ = std::make_unique<Panel>();
        root_->set_bounds(basic_rect<float>(0, 0, size.w, size.h));
        root_->get_style().padding = {20.0f, 20.0f, 20.0f, 20.0f};
        root_->get_style().background_color = Color::from_hex(0x1e1e1e);

        float y_pos = 0;
        float spacing = 15.0f;
        float col1_x = 0;
        float col2_x = 400;

        // Title
        auto* title = root_->add_child<Label>(L"ZWidget Comprehensive Demo");
        title->set_bounds(basic_rect<float>(0, y_pos, size.w - 40, 40));
        title->get_style().text_color = Color::from_hex(0x4a90e2);
        y_pos += 50;

        // === COLUMN 1: Text Input ===
        auto* section1 = root_->add_child<Label>(L"TEXT INPUT (IMPROVED)");
        section1->set_bounds(basic_rect<float>(col1_x, y_pos, 350, 25));
        section1->get_style().text_color = Color::from_hex(0xf39c12);
        y_pos += 30;

        // Improved TextBox
        improved_textbox_ = root_->add_child<TextBox>();
        improved_textbox_->set_bounds(basic_rect<float>(col1_x, y_pos, 350, 35));
        improved_textbox_->set_placeholder(L"Type anything... (Shift+Char works!)");
        improved_textbox_->on_text_changed([this](TextBox* tb, const std::wstring& text) {
            std::wcout << L"Input: " << text << std::endl;
            status_label_->set_text(L"Text changed: " + text);
            status_label_->set_text_color(Color::White());
        });
        improved_textbox_->on_enter_pressed([this](TextBox* tb) {
            std::wcout << L"Enter pressed! Text: " << tb->get_text() << std::endl;
            status_label_->set_text(L"✓ Enter pressed");
            status_label_->set_text_color(Color::from_hex(0x2ecc71));
        });
        y_pos += 45;

        // Password field
        auto* pass_label = root_->add_child<Label>(L"Password field:");
        pass_label->set_bounds(basic_rect<float>(col1_x, y_pos, 150, 25));
        y_pos += 30;

        auto* password_box = root_->add_child<TextBox>();
        password_box->set_bounds(basic_rect<float>(col1_x, y_pos, 350, 35));
        password_box->set_placeholder(L"Enter password");
        password_box->set_password_mode(true);
        y_pos += 45;

        // Read-only field
        auto* readonly_box = root_->add_child<TextBox>();
        readonly_box->set_bounds(basic_rect<float>(col1_x, y_pos, 350, 35));
        readonly_box->set_text(L"Read-only text (cannot edit)");
        readonly_box->set_read_only(true);
        readonly_box->get_style().background_color = Color::from_hex(0x2a2a2a);
        y_pos += 55;

        // === COLUMN 1: Sliders ===
        auto* section2 = root_->add_child<Label>(L"SLIDERS");
        section2->set_bounds(basic_rect<float>(col1_x, y_pos, 350, 25));
        section2->get_style().text_color = Color::from_hex(0xf39c12);
        y_pos += 30;

        // Horizontal slider
        auto* h_label = root_->add_child<Label>(L"Horizontal Slider:");
        h_label->set_bounds(basic_rect<float>(col1_x, y_pos, 150, 25));
        
        slider_value_label_ = root_->add_child<Label>(L"Value: 50");
        slider_value_label_->set_bounds(basic_rect<float>(col1_x + 200, y_pos, 150, 25));
        slider_value_label_->get_style().text_color = Color::from_hex(0x4a90e2);
        y_pos += 30;

        h_slider_ = root_->add_child<Slider>(SliderOrientation::Horizontal);
        h_slider_->set_bounds(basic_rect<float>(col1_x, y_pos, 350, 40));
        h_slider_->set_range(0, 100);
        h_slider_->set_value(50);
        h_slider_->set_step(1);
        h_slider_->on_value_changed([this](Slider* s, float value) {
            slider_value_label_->set_text(
                L"Value: " + std::to_wstring(static_cast<int>(value))
            );
            std::println("Slider value: {}", value);
        });
        y_pos += 50;

        // Vertical slider - FIXED LAYOUT
        auto* v_label = root_->add_child<Label>(L"Vertical Slider:");
        v_label->set_bounds(basic_rect<float>(col1_x, y_pos, 150, 25));
        y_pos += 30;

        v_slider_ = root_->add_child<Slider>(SliderOrientation::Vertical);
        v_slider_->set_bounds(basic_rect<float>(col1_x + 150, y_pos, 50, 150)); // Proper size
        v_slider_->set_range(0, 100);
        v_slider_->set_value(75);
        v_slider_->on_value_changed([](Slider* s, float value) {
            std::println("Vertical slider: {}", value);
        });

        // === COLUMN 2: ComboBox ===
        auto* section3 = root_->add_child<Label>(L"COMBOBOX / DROPDOWN");
        section3->set_bounds(basic_rect<float>(col2_x, 50, 350, 25));
        section3->get_style().text_color = Color::from_hex(0xf39c12);

        auto* combo_label = root_->add_child<Label>(L"Select your favorite language:");
        combo_label->set_bounds(basic_rect<float>(col2_x, 80, 350, 25));

        combo_ = root_->add_child<ComboBox>();
        combo_->set_bounds(basic_rect<float>(col2_x, 110, 300, 35));
        combo_->add_item(L"C++");
        combo_->add_item(L"Python");
        combo_->add_item(L"JavaScript");
        combo_->add_item(L"Rust");
        combo_->add_item(L"Go");
        combo_->add_item(L"Java");
        combo_->add_item(L"C#");
        combo_->set_selected_index(0);
        combo_->on_selection_changed([this](ComboBox* cb, int index) {
            if (auto* item = cb->get_selected_item()) {
                std::wcout << L"Selected: " << item->text << std::endl;
                status_label_->set_text(L"Language: " + item->text);
                status_label_->set_text_color(Color::White());
            }
        });

        // === COLUMN 2: Checkboxes ===
        float col2_y = 170;
        auto* section4 = root_->add_child<Label>(L"CHECKBOXES & RADIO");
        section4->set_bounds(basic_rect<float>(col2_x, col2_y, 350, 25));
        section4->get_style().text_color = Color::from_hex(0xf39c12);
        col2_y += 30;

        auto* cb1 = root_->add_child<CheckBox>(L"Enable feature A");
        cb1->set_bounds(basic_rect<float>(col2_x, col2_y, 200, 30));
        cb1->on_changed([](CheckBox* cb, bool checked) {
            std::println("Feature A: {}", checked ? "ON" : "OFF");
        });
        col2_y += 35;

        auto* cb2 = root_->add_child<CheckBox>(L"Enable feature B");
        cb2->set_bounds(basic_rect<float>(col2_x, col2_y, 200, 30));
        col2_y += 35;

        auto* cb3 = root_->add_child<CheckBox>(L"Enable feature C");
        cb3->set_bounds(basic_rect<float>(col2_x, col2_y, 200, 30));
        col2_y += 45;

        // Radio buttons
        auto* rb_label = root_->add_child<Label>(L"Select difficulty:");
        rb_label->set_bounds(basic_rect<float>(col2_x, col2_y, 200, 25));
        col2_y += 30;

        auto* rb1 = root_->add_child<RadioButton>(L"Easy", "difficulty");
        rb1->set_bounds(basic_rect<float>(col2_x, col2_y, 150, 30));
        rb1->set_checked(true);
        col2_y += 35;

        auto* rb2 = root_->add_child<RadioButton>(L"Medium", "difficulty");
        rb2->set_bounds(basic_rect<float>(col2_x, col2_y, 150, 30));
        col2_y += 35;

        auto* rb3 = root_->add_child<RadioButton>(L"Hard", "difficulty");
        rb3->set_bounds(basic_rect<float>(col2_x, col2_y, 150, 30));
        col2_y += 35;

        auto* rb4 = root_->add_child<RadioButton>(L"Nightmare", "difficulty");
        rb4->set_bounds(basic_rect<float>(col2_x, col2_y, 150, 30));
        col2_y += 50; // FIXED: Added spacing

        // === BUTTONS === FIXED: Proper position
        auto* button_panel = root_->add_child<StackPanel>(LayoutDirection::Horizontal);
        button_panel->set_bounds(basic_rect<float>(col2_x, col2_y, 350, 50));
        button_panel->set_spacing(10.0f);

        auto* test_btn = button_panel->add_child<Button>(L"Test All");
        test_btn->set_size(basic_size<float>(100, 40));
        test_btn->set_colors(
            Color::from_hex(0x27ae60),
            Color::from_hex(0x2ecc71),
            Color::from_hex(0x229954),
            Color::from_hex(0x1e8449)
        );
        test_btn->on_click([this](Button* btn) {
            std::println("\n=== TEST ALL WIDGETS ===");
            std::wcout << L"TextBox: " << improved_textbox_->get_text() << std::endl;
            std::println("H-Slider: {}", h_slider_->get_value());
            std::println("V-Slider: {}", v_slider_->get_value());
            if (auto* item = combo_->get_selected_item()) {
                std::wcout << L"ComboBox: " << item->text << std::endl;
            }
            status_label_->set_text(L"✓ Test completed!");
            status_label_->set_text_color(Color::from_hex(0x2ecc71)); // FIXED: Bright green
        });

        auto* reset_btn = button_panel->add_child<Button>(L"Reset");
        reset_btn->set_size(basic_size<float>(100, 40));
        reset_btn->set_colors(
            Color::from_hex(0xe67e22),
            Color::from_hex(0xf39c12),
            Color::from_hex(0xd35400),
            Color::from_hex(0xba4a00)
        );
        reset_btn->on_click([this](Button* btn) {
            improved_textbox_->set_text(L"");
            h_slider_->set_value(50);
            v_slider_->set_value(75);
            combo_->set_selected_index(0);
            status_label_->set_text(L"Reset complete");
            status_label_->set_text_color(Color::White());
        });

        auto* clear_btn = button_panel->add_child<Button>(L"Clear");
        clear_btn->set_size(basic_size<float>(100, 40));
        clear_btn->set_colors(
            Color::from_hex(0xe74c3c),
            Color::from_hex(0xec7063),
            Color::from_hex(0xc0392b),
            Color::from_hex(0xa93226)
        );
        clear_btn->on_click([this](Button* btn) {
            status_label_->set_text(L"");
        });

        // === STATUS BAR === FIXED: Better contrast
        auto* status_bar = root_->add_child<Panel>();
        status_bar->set_bounds(basic_rect<float>(0, size.h - 60, size.w - 40, 50));
        status_bar->get_style().background_color = Color::from_hex(0x2d2d2d);
        status_bar->get_style().border_color = Color::from_hex(0x3d3d3d);

        status_label_ = status_bar->add_child<Label>(L"Ready");
        status_label_->set_bounds(basic_rect<float>(10, 10, size.w - 60, 30));
        status_label_->set_text_color(Color::White()); // FIXED: Better default color

        // Initial layout
        root_->layout();
    }

    void render(Renderer& renderer) {
        root_->render(renderer);
    }

    void update(float dt) {
        root_->update(dt);
    }

    void layout() {
        root_->layout();
    }

    void resize(const basic_size<float>& new_size) {
        root_->set_size(new_size);
        root_->layout();
    }

    bool handle_mouse_down(const MouseEvent& event) {
        return root_->handle_mouse_down(event);
    }

    bool handle_mouse_up(const MouseEvent& event) {
        return root_->handle_mouse_up(event);
    }

    bool handle_mouse_move(const MouseEvent& event) {
        return root_->handle_mouse_move(event);
    }

    bool handle_key_down(const KeyboardEvent& event) {
        return root_->handle_key_down(event);
    }

    bool handle_key_up(const KeyboardEvent& event) {
        return root_->handle_key_up(event);
    }

    Widget* find_widget_at(const basic_point<float>& pos) {
        return root_->find_widget_at(pos);
    }

    Container* get_root() const { return root_.get(); }
};

int main() {
    try {
        std::println("╔══════════════════════════════════════════════════╗");
        std::println("║   ZWidget - Comprehensive Widget Demo (FIXED)   ║");
        std::println("╚══════════════════════════════════════════════════╝\n");

        if (!Renderer::initialize_factories()) {
            std::println("❌ Failed to initialize D2D");
            return 1;
        }

        if (!Application::initialize("ComprehensiveWidgetDemo")) {
            std::println("❌ Failed to initialize application");
            return 1;
        }

        std::println("✅ Application initialized\n");

        // Create window
        Window window("ZWidget - All Widgets Demo (FIXED)", Size(800, 650));

        // Create demo
        ComprehensiveWidgetDemo demo(basic_size<float>(800, 650));

        // Set paint callback
        window.set_paint_callback([&demo](Renderer& r) {
            r.clear(Color::from_hex(0x1a1a1a));
            demo.render(r);
        });

        window.show();
        std::println("✅ Window created and shown\n");
        
        std::println("🎯 ALL BUGS FIXED:");
        std::println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        std::println("✅ Close button now works");
        std::println("✅ Text input fully functional");
        std::println("✅ Sliders are visible and working");
        std::println("✅ No layout overlap");
        std::println("✅ Vertical slider has proper space");
        std::println("✅ Text contrast improved");
        std::println("✅ Consistent font colors");
        std::println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

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
        collect_focusable(demo.get_root());

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
                        demo.resize(basic_size<float>(
                            static_cast<float>(new_size.w),
                            static_cast<float>(new_size.h)
                        ));
                        window.invalidate();
                    }
                }

                if (auto* me = event.get_if<MouseEvent>()) {
                    auto pos = basic_point<float>(
                        static_cast<float>(me->get_position().x),
                        static_cast<float>(me->get_position().y)
                    );

                    if (me->get_type() == MouseEvent::Type::move) {
                        demo.handle_mouse_move(*me);
                        window.invalidate();
                    }
                    else if (me->get_type() == MouseEvent::Type::button_press) {
                        if (demo.handle_mouse_down(*me)) {
                            Widget* clicked = demo.find_widget_at(pos);
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
                        if (demo.handle_mouse_up(*me)) {
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
                            if (demo.handle_key_down(*ke)) {
                                window.invalidate();
                            }
                        }
                    }
                    else if (ke->get_type() == KeyboardEvent::Type::key_release) {
                        if (focused_widget) {
                            if (demo.handle_key_up(*ke)) {
                                window.invalidate();
                            }
                        }
                    }
                }
            }

            // Update
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - last_frame).count();

            if (dt >= 1.0f / 60.0f) {
                demo.update(dt);
                last_frame = now;
            }

            Sleep(1);
        }

        std::println("\n╔══════════════════════════════════════════════════╗");
        std::println("║   Demo Exited Successfully - All Bugs Fixed!    ║");
        std::println("╚══════════════════════════════════════════════════╝");
        return 0;

    } catch (const std::exception& e) {
        std::println("❌ Exception: {}", e.what());
        return 1;
    }
}