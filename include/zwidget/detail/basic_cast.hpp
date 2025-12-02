#pragma once

#include "zwidget/unit/point.hpp"
#include "zwidget/unit/size.hpp"

namespace zuu::widget {

	template <std::arithmetic To, std::arithmetic From>
	constexpr auto point_cast(const basic_point<From>& pt) noexcept {
		return basic_point<To>{static_cast<To>(pt.x), static_cast<To>(pt.y)} ;
	}

	template <std::arithmetic To, std::arithmetic From>
	constexpr auto point_cast(const basic_size<From>& sz) noexcept {
		return basic_point<To>{static_cast<To>(sz.w), static_cast<To>(sz.h)} ;
	}

	template <std::arithmetic To, std::arithmetic From>
	constexpr auto size_cast(const basic_size<From>& sz) noexcept {
		return basic_size<To>{static_cast<To>(sz.w), static_cast<To>(sz.h)} ;
	}

	template <std::arithmetic To, std::arithmetic From>
	constexpr auto size_cast(const basic_point<From>& pt) noexcept {
		return basic_size<To>{static_cast<To>(pt.x), static_cast<To>(pt.y)} ;
	}

} // namespace zuu::widget