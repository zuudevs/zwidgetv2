#pragma once

#include "zwidget/unit/event.hpp"
#include "event_translater.hpp"
#include <queue>
#include <Windows.h>

namespace zuu::widget {

    class EventDispatcher {
    private:
        static inline std::queue<Event> event_queue{};

    public:
        EventDispatcher() = default;
        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;
        ~EventDispatcher() = default;

        static void push_event(const Event& event) {
            event_queue.push(event);
        }

        static bool is_empty() {
            return event_queue.empty();
        }

        static Event pop_event() {
            if (event_queue.empty()) {
                return Event{};
            }
            Event event = event_queue.front();
            event_queue.pop();
            return event;
        }
        
        static auto size() {
            return event_queue.size();
        }

        static void clear() {
            std::queue<Event> empty;
            std::swap(event_queue, empty);
        }

        static bool PollEvent(Event& out_event) {
            if (!event_queue.empty()) {
                out_event = pop_event();
                return true;
            }

            MSG msg = {};
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    EventDispatcher::push_event(Event::create_quit_event());
                    out_event = pop_event();
                    return true;
                }

                Event event = detail::CreateEventFromMSG(msg);
                if (event.get_type() != Event::Type::none) {
                    EventDispatcher::push_event(event);
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (!event_queue.empty()) {
                out_event = pop_event();
                return true;
            }

            return false;
        }

        static bool WaitEvent(Event& out_event) {
            // Jika ada event di queue, langsung return
            if (!event_queue.empty()) {
                out_event = pop_event();
                return true;
            }
            
            MSG msg = {};
            BOOL result = GetMessage(&msg, nullptr, 0, 0);
            
            if (result > 0) {
                if (msg.message == WM_QUIT) {
                    EventDispatcher::push_event(Event::create_quit_event());
                    out_event = pop_event();
                    return true;
                }

                Event event = detail::CreateEventFromMSG(msg);
                if (event.get_type() != Event::Type::none) {
                    EventDispatcher::push_event(event);
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
                return PollEvent(out_event);
            } 
            else if (result == 0) {
                EventDispatcher::push_event(Event::create_quit_event());
                out_event = pop_event();
                return true;
            }
            
            return false;
        }
    };

} // namespace zuu::widget