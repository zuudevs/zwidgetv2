#pragma once

#include "zwidget/unit/event.hpp"
#include "event_translator.hpp"
#include "application.hpp"
#include <queue>
#include <mutex>
#include <Windows.h>

namespace zuu::widget {

    class EventDispatcher {
    private:
        static inline std::queue<Event> event_queue_{};
        static inline std::mutex queue_mutex_{};

    public:
        EventDispatcher() = default;
        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;
        ~EventDispatcher() = default;

        static void push_event(const Event& event) {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            event_queue_.push(event);
        }

        static bool is_empty() {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            return event_queue_.empty();
        }

        static Event pop_event() {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (event_queue_.empty()) {
                return Event{};
            }
            Event event = event_queue_.front();
            event_queue_.pop();
            return event;
        }
        
        static size_t size() {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            return event_queue_.size();
        }

        static void clear() {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            std::queue<Event> empty;
            std::swap(event_queue_, empty);
        }

        static bool PollEvent(Event& out_event) {
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                if (!event_queue_.empty()) {
                    out_event = event_queue_.front();
                    event_queue_.pop();
                    return true;
                }
            }

            MSG msg = {};
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    push_event(Event::create_quit_event());
                    
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    out_event = event_queue_.front();
                    event_queue_.pop();
                    return true;
                }

                // Get Window from HWND
                Window* window = Application::get_window(msg.hwnd);
                Event event = detail::CreateEventFromMSG(window, msg);
                if (event.get_type() != Event::Type::none) {
                    push_event(event);
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                if (!event_queue_.empty()) {
                    out_event = event_queue_.front();
                    event_queue_.pop();
                    return true;
                }
            }

            return false;
        }

        static bool WaitEvent(Event& out_event) {
			{
				std::lock_guard<std::mutex> lock(queue_mutex_);
				if (!event_queue_.empty()) {
					out_event = event_queue_.front();
					event_queue_.pop();
					return true;
				}
			}
			
			MSG msg = {};
			BOOL result = GetMessage(&msg, nullptr, 0, 0);
			
			if (result > 0) {
				if (msg.message == WM_QUIT) {
					push_event(Event::create_quit_event());
					
					std::lock_guard<std::mutex> lock(queue_mutex_);
					out_event = event_queue_.front();
					event_queue_.pop();
					return true;
				}

				Window* window = Application::get_window(msg.hwnd);
				Event event = detail::CreateEventFromMSG(window, msg);
				if (event.get_type() != Event::Type::none) {
					push_event(event);
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
				
				return PollEvent(out_event);
			} 
			else if (result == 0) {
				push_event(Event::create_quit_event());
				
				std::lock_guard<std::mutex> lock(queue_mutex_);
				out_event = event_queue_.front();
				event_queue_.pop();
				return true;
			}
			else {
				// result == -1: Error occurred
				DWORD error = GetLastError();
				// Log error or handle it appropriately
				return false;
			}
		}
    };

} // namespace zuu::widget