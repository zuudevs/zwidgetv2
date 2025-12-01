#pragma once

#include "zwidget/detail/numeric.hpp"
#include <compare>

namespace zuu::widget {

	template <std::arithmetic T>
	class basic_point {
	public :
		using value_t = T ;

		T x {} ;
		T y {} ;

		constexpr basic_point() noexcept = default ;
		constexpr basic_point(const basic_point&) noexcept = default ;
		constexpr basic_point(basic_point&&) noexcept = default ;
		constexpr basic_point& operator=(const basic_point&) noexcept = default ;
		constexpr basic_point& operator=(basic_point&&) noexcept = default ;
		constexpr std::strong_ordering operator<=>(const basic_point&) const noexcept = default ;
		constexpr ~basic_point() noexcept = default ;

		template <std::arithmetic Val>
		constexpr explicit basic_point(Val val) noexcept
		 : x(static_cast<T>(val)), y(static_cast<T>(val)) {}

		template <std::arithmetic X, std::arithmetic Y>
		constexpr basic_point(X x, Y y) noexcept
		 : x(static_cast<T>(x)), y(static_cast<T>(y)) {}

		template <std::arithmetic Val>
		constexpr basic_point& operator=(Val val) noexcept {
			x = y = static_cast<T>(val) ;
			return *this ;
		}

		template <std::arithmetic O>
		constexpr operator basic_point<O>() const noexcept {
			return basic_point<O>{x, y} ;
		}

		constexpr bool is_empty() const noexcept {
			return x == T{} && y == T{} ;
		}

		template <std::arithmetic U>
		constexpr basic_point& operator+=(const basic_point<U>& o) noexcept {
			x += static_cast<T>(o.x) ;
			y += static_cast<T>(o.y) ;
			return *this ;
		}

		template <std::arithmetic U>
		constexpr basic_point& operator-=(const basic_point<U>& o) noexcept {
			x -= static_cast<T>(o.x) ;
			y -= static_cast<T>(o.y) ;
			return *this ;
		}

		template <std::arithmetic U>
		constexpr basic_point& operator*=(const basic_point<U>& o) noexcept {
			x *= static_cast<T>(o.x) ;
			y *= static_cast<T>(o.y) ;
			return *this ;
		}

		template <std::arithmetic U>
		constexpr basic_point& operator/=(const basic_point<U>& o) noexcept {
			x /= static_cast<T>(o.x) ;
			y /= static_cast<T>(o.y) ;
			return *this ;
		}

		constexpr basic_point& operator+=(std::arithmetic auto val) noexcept {
			x += static_cast<T>(val) ;
			y += static_cast<T>(val) ;
			return *this ;
		}

		constexpr basic_point& operator-=(std::arithmetic auto val) noexcept {
			x -= static_cast<T>(val) ;
			y -= static_cast<T>(val) ;
			return *this ;
		}

		constexpr basic_point& operator*=(std::arithmetic auto val) noexcept {
			x *= static_cast<T>(val) ;
			y *= static_cast<T>(val) ;
			return *this ;
		}

		constexpr basic_point& operator/=(std::arithmetic auto val) noexcept {
			x /= static_cast<T>(val) ;
			y /= static_cast<T>(val) ;
			return *this ;
		}
	} ;

	template <typename X, typename Y>
	basic_point(X, Y) -> basic_point<std::common_type_t<X, Y>> ;

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator+(const basic_point<Lhs>& lhs, const basic_point<Rhs>& rhs) noexcept {
		return basic_point(lhs.x + rhs.x, lhs.y + rhs.y) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator-(const basic_point<Lhs>& lhs, const basic_point<Rhs>& rhs) noexcept {
		return basic_point(lhs.x - rhs.x, lhs.y - rhs.y) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator*(const basic_point<Lhs>& lhs, const basic_point<Rhs>& rhs) noexcept {
		return basic_point(lhs.x * rhs.x, lhs.y * rhs.y) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator/(const basic_point<Lhs>& lhs, const basic_point<Rhs>& rhs) noexcept {
		return basic_point(lhs.x / rhs.x, lhs.y / rhs.y) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator+(const basic_point<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_point(lhs.x + rhs, lhs.y + rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator-(const basic_point<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_point(lhs.x - rhs, lhs.y - rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator*(const basic_point<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_point(lhs.x * rhs, lhs.y * rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator/(const basic_point<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_point(lhs.x / rhs, lhs.y / rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator+(Lhs lhs, const basic_point<Rhs>& rhs) noexcept {
		return rhs + lhs ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator-(Lhs lhs, const basic_point<Rhs>& rhs) noexcept {
		return basic_point(lhs - rhs.x, lhs - rhs.y) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator*(Lhs lhs, const basic_point<Rhs>& rhs) noexcept {
		return rhs * lhs ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator/(Lhs lhs, const basic_point<Rhs>& rhs) noexcept {
		return basic_point(lhs / rhs.x, lhs / rhs.y) ;
	}

	using Point = basic_point<int32_t> ;
	using Pointf = basic_point<float> ;
	using Pointd = basic_point<double> ;

} // namespace zuu::widget
