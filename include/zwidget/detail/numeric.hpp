#pragma once

#include <compare>
#include <type_traits>

namespace std {

	template <typename T>
	concept arithmetic = std::is_arithmetic_v<T> ;

} // namespace std
