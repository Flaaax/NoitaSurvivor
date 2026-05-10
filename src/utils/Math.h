#pragma once
#include"Vec2.h"


namespace Util {
	inline static constexpr float PI = 3.1415F;

	template<vec2_t _to, vec2_t _from>
	static _to to(const _from vec) {
		return _to{ vec.x,vec.y };
	}

	template<class _to, class _from>
	static sf::Vector2<_to> to(const sf::Vector2<_from>& v) {
		return { static_cast<_to>(v.x),static_cast<_to>(v.y) };
	}

	template<vec2_t _vec2>
	static _vec2 devide(const _vec2& v, float devisor) {
		return _vec2{ v.x / devisor,v.y / devisor };
	}

	template<vec2_t _vec2>
	static _vec2 by(const _vec2& v, float factor) {
		return _vec2{ v.x * factor,v.y * factor };
	}

	template<vec2_t _vec2>
	static _vec2 norm(const _vec2& v) {
		float length = std::sqrt(v.x * v.x + v.y * v.y);
		if (length != 0.F) return { v.x / length, v.y / length };
		else return { 0.F, 0.F };
	}

	template<vec2_t _vec2>
	static float length(const _vec2& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	// as radiant
	template<vec2_t _vec2>
	static float to_rad(const _vec2& vec) {
		return std::atan2(vec.y, vec.x);
	}

	template<vec2_t _vec2>
	static _vec2 from_rad(float arg) {
		return { std::cos(arg), std::sin(arg) };
	}

	static float to_rad(float deg) {
		return deg * PI / 180;
	}

	static float to_deg(float rad) {
		return rad * 180 / PI;
	}

	template<vec2_t _vec2 = bvec2>
	static _vec2 velocity(float arg, float speed) {
		return speed * from_rad<_vec2>(arg);
	}

	template<vec2_t _vec2>
	static _vec2 velocity(const _vec2& dir, float speed) {
		return speed * norm(dir);
	}

	template <typename T>requires
		requires (T a, T b) { { a > b } -> std::convertible_to<bool>; }
	static T max(const T& a, const T& b) {
		return (a > b) ? a : b;
	}

	template <typename T>requires
		requires (T a, T b) { { a > b } -> std::convertible_to<bool>; }
	static T min(const T& a, const T& b) {
		return (a < b) ? a : b;
	}
};
