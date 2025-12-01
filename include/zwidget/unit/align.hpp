#pragma once

#include <cstdint>

namespace zuu::widget {

	enum class QAlign : uint8_t { // alignment qualifier
		none = 0,
		start = 1 << 0,
		center = 1 << 1,
		end = 1 << 2,
	} ;

} // namespace zuu::widget