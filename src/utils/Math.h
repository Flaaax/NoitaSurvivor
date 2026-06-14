#pragma once
#include "Vec2/Vec2.h"

namespace flx::math {
	inline static constexpr float PI = 3.1415F;

	// as radiant
	static float to_rad(vec2 vec) {
		return std::atan2(vec.y, vec.x);
	}

	static vec2 from_rad(float arg) {
		return {std::cos(arg), std::sin(arg)};
	}

	static float to_rad(float deg) {
		return deg * PI / 180;
	}

	static float to_deg(float rad) {
		return rad * 180 / PI;
	}

	static vec2 velocity(float arg, float speed) {
		return speed * from_rad(arg);
	}
} // namespace flx::math
