#pragma once

#include "zwidget/unit/size.hpp"
#include <cstdint>
namespace zuu::widget {

	class WindowEvent {
	public :
		enum class Type : uint8_t {
			none,
			close,
			focus_gained,
			focus_lost,
			resize,
			minimize,
			maximize,
			restored,
		} ;

	private :
		Type type {} ;					//  1 byte
		uint8_t padding[1] {} ;			//  1 byte
		basic_size<uint16_t> size {} ;	//  4 byte
		// --------------------------------------------
		// Total : 							6 bytes

	public :
		constexpr WindowEvent() noexcept = default ;
		constexpr WindowEvent(const WindowEvent&) noexcept = default ;
		constexpr WindowEvent(WindowEvent&&) noexcept = default ;
		constexpr WindowEvent& operator=(const WindowEvent&) noexcept = default ;
		constexpr WindowEvent& operator=(WindowEvent&&) noexcept = default ;
		constexpr bool operator==(const WindowEvent&) const noexcept = default ;
		constexpr bool operator!=(const WindowEvent&) const noexcept = default ;
		constexpr ~WindowEvent() noexcept = default ;

		constexpr WindowEvent(Type type_) noexcept
		 : type(type_) {}

		constexpr WindowEvent(Type type_, const basic_size<uint16_t>& size_) noexcept
		 : type(type_), size(size_) {}

		constexpr Type get_type() const noexcept {
			return type ;
		}

		constexpr const basic_size<uint16_t>& get_size() const noexcept {
			return size ;
		}
	} ;

} // namespace zuu::widget