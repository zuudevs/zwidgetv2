#pragma once

#include "zwidget/unit/point.hpp"
#include "zwidget/unit/size.hpp"

namespace zuu::widget {

	template <std::arithmetic T>
	class basic_rect : public basic_point<T>, public basic_size<T> {
	public :
		constexpr basic_rect() noexcept = default ;
		constexpr basic_rect(const basic_rect&) noexcept = default ;
		constexpr basic_rect(basic_rect&&) noexcept = default ;
		constexpr basic_rect& operator=(const basic_rect&) noexcept = default ;
		constexpr basic_rect& operator=(basic_rect&&) noexcept = default ;
		constexpr std::strong_ordering operator<=>(const basic_rect&) const noexcept = default ;

		template <std::arithmetic Val>
		constexpr explicit basic_rect(Val val) noexcept
		 : basic_point<T>(val), basic_size<T>(val) {}

		template <std::arithmetic U, std::arithmetic V>
		constexpr explicit basic_rect(const basic_point<U>& point, const basic_size<V>& size) noexcept
		 : basic_point<T>(point), basic_size<T>(size) {}

		constexpr basic_rect(
			std::arithmetic auto x_,
			std::arithmetic auto y_,
			std::arithmetic auto w_,
			std::arithmetic auto h_
		) noexcept
		 : basic_point<T>(x_, y_), basic_size<T>(w_, h_) {}

		template <std::arithmetic Val>
		constexpr basic_rect& operator=(Val val) noexcept {
			get_point() = val ;
			get_size() = val ;
			return *this ;
		}

		constexpr auto& get_point() noexcept {
			return static_cast<basic_point<T>&>(*this) ;
		}

		constexpr auto& get_size() noexcept {
			return static_cast<basic_size<T>&>(*this) ;
		}

		constexpr auto& get_point() const noexcept {
			return static_cast<const basic_point<T>&>(*this) ;
		}

		constexpr auto& get_size() const noexcept {
			return static_cast<const basic_size<T>&>(*this) ;
		}

		template <std::arithmetic U>
		constexpr basic_rect& operator+=(const basic_rect<U>& o) noexcept {
			get_point() += o.get_point() ;
			get_size() += o.get_size() ;
			return *this ;
		}

		template <std::arithmetic U>
		constexpr basic_rect& operator-=(const basic_rect<U>& o) noexcept {
			get_point() -= o.get_point() ;
			get_size() -= o.get_size() ;
			return *this ;
		}

		template <std::arithmetic U>
		constexpr basic_rect& operator*=(const basic_rect<U>& o) noexcept {
			get_point() *= o.get_point() ;
			get_size() *= o.get_size() ;
			return *this ;
		}

		template <std::arithmetic U>
		constexpr basic_rect& operator/=(const basic_rect<U>& o) noexcept {
			get_point() /= o.get_point() ;
			get_size() /= o.get_size() ;
			return *this ;
		}

		template <std::arithmetic Val>
		constexpr basic_rect& operator+=(Val val) noexcept {
			get_point() += val ;
			get_size() += val ;
			return *this ;
		}

		template <std::arithmetic Val>
		constexpr basic_rect& operator-=(Val val) noexcept {
			get_point() -= val ;
			get_size() -= val ;
			return *this ;
		}

		template <std::arithmetic Val>
		constexpr basic_rect& operator*=(Val val) noexcept {
			get_point() *= val ;
			get_size() *= val ;
			return *this ;
		}

		template <std::arithmetic Val>
		constexpr basic_rect& operator/=(Val val) noexcept {
			get_point() /= val ;
			get_size() /= val ;
			return *this ;
		}
	} ;

	template <typename U, typename V>
	basic_rect(basic_point<U>, basic_size<V>) -> basic_rect<std::common_type_t<U, V>> ;
	
	template <typename X, typename Y, typename W, typename H>
	basic_rect(X, Y, W, H) -> basic_rect<std::common_type_t<X, Y, W, H>> ;

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator+(const basic_rect<Lhs>& lhs, const basic_rect<Rhs>& rhs) noexcept {
		return basic_rect(lhs.get_point() + rhs.get_point(), lhs.get_size() + rhs.get_size()) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator-(const basic_rect<Lhs>& lhs, const basic_rect<Rhs>& rhs) noexcept {
		return basic_rect(lhs.get_point() - rhs.get_point(), lhs.get_size() - rhs.get_size()) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator*(const basic_rect<Lhs>& lhs, const basic_rect<Rhs>& rhs) noexcept {
		return basic_rect(lhs.get_point() * rhs.get_point(), lhs.get_size() * rhs.get_size()) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator/(const basic_rect<Lhs>& lhs, const basic_rect<Rhs>& rhs) noexcept {
		return basic_rect(lhs.get_point() / rhs.get_point(), lhs.get_size() / rhs.get_size()) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator+(const basic_rect<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_rect(lhs.get_point() + rhs, lhs.get_size() + rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator-(const basic_rect<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_rect(lhs.get_point() - rhs, lhs.get_size() - rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator*(const basic_rect<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_rect(lhs.get_point() * rhs, lhs.get_size() * rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator/(const basic_rect<Lhs>& lhs, Rhs rhs) noexcept {
		return basic_rect(lhs.get_point() / rhs, lhs.get_size() / rhs) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator+(Lhs lhs, const basic_rect<Rhs>& rhs) noexcept {
		return rhs + lhs ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator-(Lhs lhs, const basic_rect<Rhs>& rhs) noexcept {
		return basic_rect(lhs - rhs.get_point(), lhs - rhs.get_size()) ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator*(Lhs lhs, const basic_rect<Rhs>& rhs) noexcept {
		return rhs * lhs ;
	}

	template <std::arithmetic Lhs, std::arithmetic Rhs>
	constexpr auto operator/(Lhs lhs, const basic_rect<Rhs>& rhs) noexcept {
		return basic_rect(lhs / rhs.get_point(), lhs / rhs.get_size()) ;
	}

	using Rect = basic_rect<int> ;
	using Rectf = basic_rect<float> ;
	using Rectd = basic_rect<double> ;

} // namespace zuu::widget
