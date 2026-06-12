#pragma once
#include "Vec2/Vec2.h"

namespace Util {
	inline static constexpr float PI = 3.1415F;

	// as radiant
	static float to_rad(nvec2 vec) {
		return std::atan2(vec.y, vec.x);
	}

	static nvec2 from_rad(float arg) {
		return {std::cos(arg), std::sin(arg)};
	}

	static float to_rad(float deg) {
		return deg * PI / 180;
	}

	static float to_deg(float rad) {
		return rad * 180 / PI;
	}

	static nvec2 velocity(float arg, float speed) {
		return speed * from_rad(arg);
	}

	template <typename T>
		requires requires(T a, T b) { { a > b } -> std::convertible_to<bool>; }
	static T max(const T& a, const T& b) {
		return (a > b) ? a : b;
	}

	template <typename T>
		requires requires(T a, T b) { { a > b } -> std::convertible_to<bool>; }
	static T min(const T& a, const T& b) {
		return (a < b) ? a : b;
	}
} // namespace Util
