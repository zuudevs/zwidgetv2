#pragma once

#include <type_traits>
#include <variant>

namespace zuu::meta {
	template <typename T, typename Variant>
	struct in_variant_type : std::false_type {} ;

	template <typename T, typename ... Ts>
	struct in_variant_type<T, std::variant<Ts...>>
	 : std::disjunction<std::is_same<T, Ts>...> {} ;

	template <typename T, typename Variant>
	constexpr bool in_variant_type_v = in_variant_type<T, Variant>::value ;

	template <typename T, typename Variant>
	concept variant_type = in_variant_type_v<T, Variant> ;
}