#pragma once

#include <cstdint>

namespace zuu::widget {

	enum class KeyMod : uint8_t {
		none		= 0,
		shift		= 1 << 0,
		ctrl		= 1 << 1,
		alt			= 1 << 2,
		super		= 1 << 3,
		capslock	= 1 << 4,
		numlock		= 1 << 5,
	} ;

	constexpr KeyMod operator|(KeyMod lhs, KeyMod rhs) noexcept {
		return static_cast<KeyMod>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)) ;
	}

	constexpr KeyMod operator&(KeyMod lhs, KeyMod rhs) noexcept {
		return static_cast<KeyMod>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)) ;
	}

	constexpr KeyMod& operator|=(KeyMod& lhs, KeyMod rhs) noexcept {
		lhs = lhs | rhs ;
		return lhs ;
	}

	constexpr KeyMod& operator&=(KeyMod& lhs, KeyMod rhs) noexcept {
		lhs = lhs & rhs ;
		return lhs ;
	}

	constexpr bool has_keymod(KeyMod mods, KeyMod check) noexcept {
		return static_cast<uint8_t>(mods & check) != 0 ;
	}

} // namespace zuu::widget