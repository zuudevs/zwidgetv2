#include "zwidget/unit/rect.hpp"
#include <print>

using namespace zuu::widget ;
using namespace std ;

template <std::arithmetic T>
inline void display(const basic_point<T>& point) noexcept {
	std::println("{}{}, {}{}", '{', point.x, point.y, '}') ;
}

template <std::arithmetic T>
inline void display(const basic_size<T>& size) noexcept {
	std::println("{}{}, {}{}", '{', size.w, size.h, '}') ;
}

template <std::arithmetic T>
inline void display(const basic_rect<T>& rect) noexcept {
	std::println("{}{}, {}, {}, {}{}", '{', rect.x, rect.y, rect.w, rect.h, '}') ;
}

int main() {
	auto p1 = Point(1, 2) ;
	auto p2 = Pointf(1.5, 2.5) ;
	auto p3 = p1 + p2 ;
	auto s1 = Size(1, 4) ;
	auto s2 = Sizef(1.6f, 5.2f) ;
	auto s3 = s1 + 2 + s2;
	auto r1 = Rect(1, 2, 3, 4) ;
	auto r2 = basic_rect(1, 2, 3, 1.5f) ;
	auto r3 = r1 + r2 + 2.5 ;

	display(p1) ;
	display(p2) ;
	display(p3) ;
	display(s1) ;
	display(s2) ;
	display(r3) ;
	display(r1) ;
	display(r2) ;
	display(r3) ;
}