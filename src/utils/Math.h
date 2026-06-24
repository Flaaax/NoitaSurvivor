#pragma once
#include "Vec2/Vec2.h"
#include <numbers>

namespace flx::math {

	inline constexpr float pi = std::numbers::pi_v<float>;

	// as radiant
	// static float to_rad(vec2 vec) {
	// 	return std::atan2(vec.y, vec.x);
	// }
	//
	// static vec2 from_rad(float arg) {
	// 	return {std::cos(arg), std::sin(arg)};
	// }

	constexpr float to_rad(float deg) {
		return deg * pi / 180;
	}

	constexpr float to_deg(float rad) {
		return rad * 180 / pi;
	}

	inline vec2 velocity(float arg, float speed) {
		return speed * vec2::rad(arg);
	}

	constexpr float sgn(float x) {
		return static_cast<float>((0.f < x) - (x < 0.f));
	}

	inline float argDiff(float a, float b) {
		return std::remainder(a - b, 2.0f * pi);
	}
} // namespace flx::math
