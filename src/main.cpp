#include "zwidget/zwidget.hpp"
#include "zwidget/widgets/label.hpp"
#include "zwidget/widgets/textbox.hpp"
#include "zwidget/widgets/button.hpp"
#include <print>

using namespace zuu::widget;

class LoginPage {
private:
    std::unique_ptr<Panel> root_;
    Panel* content_panel_{ nullptr };
    
    // Widget components
    Label* title_label_{ nullptr };
    Label* username_label_{ nullptr };
    Label* password_label_{ nullptr };
    TextBox* username_tb_{ nullptr };
    TextBox* password_tb_{ nullptr };
    Button* login_btn_{ nullptr };

public:
    LoginPage(const basic_size<float>& size) {
        // 1. Setup Root Panel (Background Gelap)
        root_ = std::make_unique<Panel>();
        root_->set_bounds(Rectf{ Pointf{0.0f}, size });
        root_->get_style().background_color = Color::from_hex(0x121212); // Lebih gelap dikit biar elegan

        // 2. Setup Content Panel (Kotak Login di tengah)
        Sizef content_size = size / Sizef{ 2.5f, 1.8f }; // Ukuran proporsional
        content_panel_ = root_->add_child<Panel>();
        content_panel_->set_bounds(Rectf{ Pointf{}, content_size });
        content_panel_->get_style().background_color = Color::from_hex(0x252526);
        content_panel_->get_style().border_radius = 8.0f; // Kasih radius biar gak kaku
        
        // Center the panel
        Pointf center_pos = point_cast<float>((size - content_size) / 2.0f);
        content_panel_->set_position(center_pos);

        // 3. Layout Logic (Manual Layout Foundation)
        // Kita atur posisi relatif terhadap content_panel_
        float padding_x = 30.0f;
        float current_y = 30.0f;
        float field_width = content_size.w - (padding_x * 2);
        float field_height = 35.0f;
        float label_height = 20.0f;
        float spacing = 10.0f;

        // --- TITLE ---
        title_label_ = content_panel_->add_child<Label>(L"WELCOME BACK");
        title_label_->set_bounds(Rectf{ padding_x, current_y, field_width, 30.0f });
        // title_label_->set_horizontal_alignment(QAlign::center); // Kalau Label support alignment
        title_label_->set_text_color(Color::from_hex(0x4a90e2)); // Biru kece
        current_y += 30.0f + spacing * 2;

        // --- USERNAME ---
        username_label_ = content_panel_->add_child<Label>(L"Username");
        username_label_->set_bounds(Rectf{ padding_x, current_y, field_width, label_height });
        username_label_->set_text_color(Color::from_hex(0xcccccc));
        current_y += label_height + 5.0f;

        username_tb_ = content_panel_->add_child<TextBox>();
        username_tb_->set_bounds(Rectf{ padding_x, current_y, field_width, field_height });
        username_tb_->set_placeholder(L"Enter your username");
        current_y += field_height + spacing;

        // --- PASSWORD ---
        password_label_ = content_panel_->add_child<Label>(L"Password");
        password_label_->set_bounds(Rectf{ padding_x, current_y, field_width, label_height });
        password_label_->set_text_color(Color::from_hex(0xcccccc));
        current_y += label_height + 5.0f;

        password_tb_ = content_panel_->add_child<TextBox>();
        password_tb_->set_bounds(Rectf{ padding_x, current_y, field_width, field_height });
        password_tb_->set_placeholder(L"Enter your password");
        password_tb_->set_password_mode(true); // Rahasia dong!
        current_y += field_height + spacing * 2;

        // --- LOGIN BUTTON ---
        login_btn_ = content_panel_->add_child<Button>(L"LOGIN");
        login_btn_->set_bounds(Rectf{ padding_x, current_y, field_width, 40.0f });
        login_btn_->set_colors(
            Color::from_hex(0x4a90e2), // Normal
            Color::from_hex(0x357abd), // Hover
            Color::from_hex(0x2a68a8), // Pressed
            Color::from_hex(0x555555)  // Disabled
        );
        
        // Button Logic
        login_btn_->on_click([this](Button* b) {
            std::println("Login Attempt: {} / {}", 
                std::string(username_tb_->get_text().begin(), username_tb_->get_text().end()),
                "******" // Jangan print password asli di log woy!
            );
        });
    }

    void render(Renderer& renderer) {
        root_->render(renderer);
    }

    // PENTING: Kamu harus oper event dari Window ke sini!
    void handle_event(const Event& ev) {
        if (!root_) return;

        // Dispatching manual karena belum ada sistem otomatis yang kompleks
        if (auto* me = ev.get_if<MouseEvent>()) {
            switch (me->get_type()) {
                case MouseEvent::Type::button_press: root_->handle_mouse_down(*me); break;
                case MouseEvent::Type::button_release: root_->handle_mouse_up(*me); break;
                case MouseEvent::Type::move: root_->handle_mouse_move(*me); break;
                // scroll dll...
                default: break;
            }
        }
        else if (auto* ke = ev.get_if<KeyboardEvent>()) {
            switch (ke->get_type()) {
                case KeyboardEvent::Type::key_press: root_->handle_key_down(*ke); break;
                case KeyboardEvent::Type::key_release: root_->handle_key_up(*ke); break;
                default: break;
            }
        }
    }
};

int main() {
    if (!Renderer::initialize_factories()) {
        std::println("Failed to initialize D2D");
        return 1;
    }

    if (!Application::initialize("TestLayout")) {
        std::println("Failed to initialize application");
        return 1;
    }

    Window window("Simple Login Layout", Size(800, 600));
    window.show();

    LoginPage login_page(basic_size<float>(800, 600));

    // Paint callback
    window.set_paint_callback([&login_page](Renderer& renderer) {
        renderer.clear(Color::from_hex(0x121212));
        login_page.render(renderer);
    });

    while (Application::is_running()) {
        Event ev;
        // Pake WaitEvent biar CPU gak kebakar pas idle
        if (EventDispatcher::WaitEvent(ev)) {
            
            // 1. Handle Window Event (Close, Resize, etc)
            if (auto* e = ev.get_if<WindowEvent>()) {
                if (e->get_type() == WindowEvent::Type::close) {
                    window.close();
                    Application::shutdown();
                }
            }
            
            // 2. TERUSKAN EVENT KE LOGIN PAGE! <--- INI KUNCINYA
            login_page.handle_event(ev);
            
            // 3. Request redraw kalau ada interaksi (optimasi kasar)
            if (ev.get_if<MouseEvent>() || ev.get_if<KeyboardEvent>()) {
                window.invalidate(); 
            }
        }
    }
    return 0;
}