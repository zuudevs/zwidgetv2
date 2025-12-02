#include "zwidget/unit/window.hpp"
#include "zwidget/core/application.hpp"
#include "zwidget/widgets/widget.hpp"
#include "zwidget/widgets/container.hpp"
#include "zwidget/widgets/button.hpp"
#include "zwidget/widgets/label.hpp"
#include "zwidget/widgets/panel.hpp"
#include "zwidget/widgets/textbox.hpp"
#include "zwidget/widgets/checkbox.hpp"
#include <iostream>
#include <print>
#include <memory>
#include <vector>

using namespace zuu::widget;

class FormDemo {
private:
    std::unique_ptr<Panel> root_;
    Label* status_label_{nullptr};
    TextBox* name_input_{nullptr};
    TextBox* password_input_{nullptr};
    CheckBox* remember_me_{nullptr};
    CheckBox* accept_terms_{nullptr};
    RadioButton* option1_{nullptr};
    RadioButton* option2_{nullptr};
    RadioButton* option3_{nullptr};
    Button* submit_btn_{nullptr};
    Button* clear_btn_{nullptr};

public:
    FormDemo(const basic_size<float>& size) {
        root_ = std::make_unique<Panel>();
        root_->set_bounds(basic_rect<float>(0, 0, size.w, size.h));
        root_->get_style().padding = {20.0f, 20.0f, 20.0f, 20.0f};
        root_->get_style().background_color = Color::from_hex(0x1e1e1e);

        float y_pos = 0;
        float spacing = 15.0f;

        // Title
        auto* title = root_->add_child<Label>(L"Registration Form");
        title->set_bounds(basic_rect<float>(0, y_pos, size.w - 40, 40));
        title->get_style().text_color = Color::from_hex(0x4a90e2);
        y_pos += 50;

        // Name input
        auto* name_label = root_->add_child<Label>(L"Username:");
        name_label->set_bounds(basic_rect<float>(0, y_pos, 150, 25));
        y_pos += 30;

        name_input_ = root_->add_child<TextBox>();
        name_input_->set_bounds(basic_rect<float>(0, y_pos, 300, 35));
        name_input_->set_placeholder(L"Enter your username");
        name_input_->on_text_changed([this](TextBox* tb, const std::wstring& text) {
            std::wcout << L"Username: " << text << std::endl;
        });
        y_pos += 45;

        // Password input
        auto* pass_label = root_->add_child<Label>(L"Password:");
        pass_label->set_bounds(basic_rect<float>(0, y_pos, 150, 25));
        y_pos += 30;

        password_input_ = root_->add_child<TextBox>();
        password_input_->set_bounds(basic_rect<float>(0, y_pos, 300, 35));
        password_input_->set_placeholder(L"Enter your password");
        password_input_->set_password_mode(true);
        password_input_->on_enter_pressed([this](TextBox* tb) {
            submit_form();
        });
        y_pos += 45;

        // Checkboxes
        remember_me_ = root_->add_child<CheckBox>(L"Remember me");
        remember_me_->set_bounds(basic_rect<float>(0, y_pos, 200, 30));
        remember_me_->on_changed([](CheckBox* cb, bool checked) {
            std::println("Remember me: {}", checked ? "ON" : "OFF");
        });
        y_pos += 35;

        accept_terms_ = root_->add_child<CheckBox>(L"I accept the terms and conditions");
        accept_terms_->set_bounds(basic_rect<float>(0, y_pos, 300, 30));
        accept_terms_->on_changed([this](CheckBox* cb, bool checked) {
            submit_btn_->set_enabled(checked);
            std::println("Terms accepted: {}", checked ? "YES" : "NO");
        });
        y_pos += 45;

        // Radio buttons
        auto* options_label = root_->add_child<Label>(L"Select your role:");
        options_label->set_bounds(basic_rect<float>(0, y_pos, 200, 25));
        y_pos += 30;

        option1_ = root_->add_child<RadioButton>(L"Student", "role");
        option1_->set_bounds(basic_rect<float>(0, y_pos, 150, 30));
        option1_->on_changed([](RadioButton* rb, bool checked) {
            if (checked) std::println("Role: Student");
        });
        y_pos += 35;

        option2_ = root_->add_child<RadioButton>(L"Teacher", "role");
        option2_->set_bounds(basic_rect<float>(0, y_pos, 150, 30));
        option2_->on_changed([](RadioButton* rb, bool checked) {
            if (checked) std::println("Role: Teacher");
        });
        y_pos += 35;

        option3_ = root_->add_child<RadioButton>(L"Administrator", "role");
        option3_->set_bounds(basic_rect<float>(0, y_pos, 200, 30));
        option3_->on_changed([](RadioButton* rb, bool checked) {
            if (checked) std::println("Role: Administrator");
        });
        y_pos += 45;

        // Buttons
        auto* button_panel = root_->add_child<StackPanel>(LayoutDirection::Horizontal);
        button_panel->set_bounds(basic_rect<float>(0, y_pos, 320, 40));
        button_panel->set_spacing(10.0f);

        submit_btn_ = button_panel->add_child<Button>(L"Submit");
        submit_btn_->set_size(basic_size<float>(150, 40));
        submit_btn_->set_enabled(false); // Enabled when terms accepted
        submit_btn_->set_colors(
            Color::from_hex(0x27ae60),
            Color::from_hex(0x2ecc71),
            Color::from_hex(0x229954),
            Color::from_hex(0x1e8449)
        );
        submit_btn_->on_click([this](Button* btn) {
            submit_form();
        });

        clear_btn_ = button_panel->add_child<Button>(L"Clear");
        clear_btn_->set_size(basic_size<float>(150, 40));
        clear_btn_->set_colors(
            Color::from_hex(0xe74c3c),
            Color::from_hex(0xec7063),
            Color::from_hex(0xc0392b),
            Color::from_hex(0xa93226)
        );
        clear_btn_->on_click([this](Button* btn) {
            clear_form();
        });

        y_pos += 55;

        // Status label
        status_label_ = root_->add_child<Label>(L"");
        status_label_->set_bounds(basic_rect<float>(0, y_pos, size.w - 40, 30));
        status_label_->set_text_color(Color::from_hex(0xf39c12));

        // Initial layout
        root_->layout();
    }

    void submit_form() {
        std::println("\n=== FORM SUBMITTED ===");
        std::wcout << L"Username: " << name_input_->get_text() << std::endl;
        std::wcout << L"Password: " << (password_input_->get_text().empty() ? 
                                       L"(empty)" : L"********") << std::endl;
        std::println("Remember me: {}", remember_me_->is_checked() ? "YES" : "NO");
        std::println("Terms accepted: {}", accept_terms_->is_checked() ? "YES" : "NO");
        
        std::print("Role: ");
        if (option1_->is_checked()) std::println("Student");
        else if (option2_->is_checked()) std::println("Teacher");
        else if (option3_->is_checked()) std::println("Administrator");
        else std::println("(not selected)");
        std::println("=====================\n");

        status_label_->set_text(L"✓ Form submitted successfully!");
        status_label_->set_text_color(Color::from_hex(0x27ae60));
    }

    void clear_form() {
        name_input_->set_text(L"");
        password_input_->set_text(L"");
        remember_me_->set_checked(false);
        accept_terms_->set_checked(false);
        option1_->set_checked(false);
        option2_->set_checked(false);
        option3_->set_checked(false);
        
        status_label_->set_text(L"Form cleared");
        status_label_->set_text_color(Color::Gray());
        
        std::println("Form cleared");
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
        std::println("=== ZWidget Comprehensive Widget Demo ===\n");

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
        Window window("Comprehensive Widget Demo", Size(500, 700));

        // Create form demo
        FormDemo demo(basic_size<float>(500, 700));

        // Set paint callback
        window.set_paint_callback([&demo](Renderer& r) {
            r.clear(Color::from_hex(0x1a1a1a));
            demo.render(r);
        });

        window.show();
        std::println("✅ Window created and shown\n");
        std::println("📝 Test the following widgets:");
        std::println("   ✓ TextBox - Type in username and password fields");
        std::println("   ✓ CheckBox - Toggle remember me and accept terms");
        std::println("   ✓ RadioButton - Select a role (mutually exclusive)");
        std::println("   ✓ Button - Submit or Clear the form");
        std::println("   ✓ Layout - Automatic widget positioning");
        std::println("\n🎯 Interactions:");
        std::println("   - Click widgets to interact");
        std::println("   - Press TAB to cycle focus");
        std::println("   - Type in text boxes");
        std::println("   - Press ENTER in password to submit");
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

        std::println("\n=== Demo Exited Successfully ===");
        return 0;

    } catch (const std::exception& e) {
        std::println("❌ Exception: {}", e.what());
        return 1;
    }
}