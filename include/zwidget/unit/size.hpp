#pragma once

#include "zwidget/detail/numeric.hpp"
#include <compare>
#include <limits>  

namespace zuu::widget {
	template <std::arithmetic T>
	class basic_point ;
	
    template <std::arithmetic T>
    class basic_size {
    public :
        using value_t = T ;
        static constexpr auto MAX = std::numeric_limits<T>::max() ;

        T w {} ;
        T h {} ;

    private:
        template <typename V>
        static constexpr T safe_clamp(V val) noexcept {
            return (val < static_cast<V>(0) ? static_cast<T>(0) : (
				val > static_cast<V>(MAX) ? MAX :static_cast<T>(val)
			)) ;
        }

    public :
        constexpr basic_size() noexcept = default ;
        constexpr basic_size(const basic_size&) noexcept = default ;
        constexpr basic_size(basic_size&&) noexcept = default ;
        constexpr basic_size& operator=(const basic_size&) noexcept = default ;
        constexpr basic_size& operator=(basic_size&&) noexcept = default ;
        constexpr std::strong_ordering operator<=>(const basic_size&) const noexcept = default ;
        constexpr ~basic_size() noexcept = default ;

        template <std::arithmetic Val>
        constexpr explicit basic_size(Val val) noexcept
         : w(safe_clamp(val)), h(safe_clamp(val)) {}

        
        template <std::arithmetic W, std::arithmetic H>
        constexpr basic_size(W w, H h) noexcept
         : w(safe_clamp(w)), h(safe_clamp(h)) {}

        template <std::arithmetic Val>
        constexpr basic_size& operator=(Val val) noexcept {
            w = h = safe_clamp(val) ;
            return *this ;
        }

        template <std::arithmetic O>
        constexpr operator basic_size<O>() const noexcept {
            return basic_size<O>{static_cast<O>(w), static_cast<O>(h)} ;
        }

		constexpr bool is_empty() const noexcept {
			return w == T{} || h == T{} ;
		}

        template <std::arithmetic U>
        constexpr basic_size& operator+=(const basic_size<U>& o) noexcept {
            w = safe_clamp(w + o.w);
            h = safe_clamp(h + o.h);
            return *this ;
        }

        template <std::arithmetic U>
        constexpr basic_size& operator-=(const basic_size<U>& o) noexcept {
            w = safe_clamp(w - o.w);
            h = safe_clamp(h - o.h);
            return *this ;
        }

        template <std::arithmetic U>
        constexpr basic_size& operator*=(const basic_size<U>& o) noexcept {
            w = safe_clamp(w * o.w);
            h = safe_clamp(h * o.h);
            return *this ;
        }

        template <std::arithmetic U>
        constexpr basic_size& operator/=(const basic_size<U>& o) noexcept {
            w = safe_clamp(w / o.w);
            h = safe_clamp(h / o.h);
            return *this ;
        }

        
        template <std::arithmetic Val>
        constexpr basic_size& operator+=(Val val) noexcept {
            w = safe_clamp(w + val);
            h = safe_clamp(h + val);
            return *this;
        }

        template <std::arithmetic Val>
        constexpr basic_size& operator-=(Val val) noexcept {
            w = safe_clamp(w - val);
            h = safe_clamp(h - val);
            return *this;
        }

        template <std::arithmetic Val>
        constexpr basic_size& operator*=(Val val) noexcept {
            w = safe_clamp(w * val);
            h = safe_clamp(h * val);
            return *this;
        }

        template <std::arithmetic Val>
        constexpr basic_size& operator/=(Val val) noexcept {
            w = safe_clamp(w / val);
            h = safe_clamp(h / val);
            return *this;
        }
    } ;

    
    template <typename X, typename Y>
    basic_size(X, Y) -> basic_size<std::common_type_t<X, Y>> ;

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator+(const basic_size<Lhs>& lhs, const basic_size<Rhs>& rhs) noexcept {
        return basic_size(lhs.w + rhs.w, lhs.h + rhs.h) ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator-(const basic_size<Lhs>& lhs, const basic_size<Rhs>& rhs) noexcept {
       return basic_size(lhs.w - rhs.w, lhs.h - rhs.h) ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator*(const basic_size<Lhs>& lhs, const basic_size<Rhs>& rhs) noexcept {
       return basic_size(lhs.w * rhs.w, lhs.h * rhs.h) ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator/(const basic_size<Lhs>& lhs, const basic_size<Rhs>& rhs) noexcept {
       return basic_size(lhs.w / rhs.w, lhs.h / rhs.h) ;
    }
       
    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator+(const basic_size<Lhs>& lhs, Rhs rhs) noexcept {
        return basic_size(lhs.w + rhs, lhs.h + rhs) ;
    }
    
    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator-(const basic_size<Lhs>& lhs, Rhs rhs) noexcept {
        return basic_size(lhs.w - rhs, lhs.h - rhs) ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator*(const basic_size<Lhs>& lhs, Rhs rhs) noexcept {
        return basic_size(lhs.w * rhs, lhs.h * rhs) ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator/(const basic_size<Lhs>& lhs, Rhs rhs) noexcept {
        return basic_size(lhs.w / rhs, lhs.h / rhs) ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator+(Lhs lhs, const basic_size<Rhs>& rhs) noexcept {
        return rhs + lhs ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator-(Lhs lhs, const basic_size<Rhs>& rhs) noexcept {
        return basic_size(lhs - rhs.w, lhs - rhs.h) ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator*(Lhs lhs, const basic_size<Rhs>& rhs) noexcept {
        return rhs * lhs ;
    }

    template <std::arithmetic Lhs, std::arithmetic Rhs>
    constexpr auto operator/(Lhs lhs, const basic_size<Rhs>& rhs) noexcept {
        return basic_size(lhs / rhs.w, lhs / rhs.h) ;
    }

	using Size = basic_size<int> ;
	using Sizef = basic_size<float> ;
	using Sized = basic_size<double> ;
	
} // namespace zuu::widget