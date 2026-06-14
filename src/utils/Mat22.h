#pragma once
#ifndef NOITASURVIVOR_MATH_H
#define NOITASURVIVOR_MATH_H
#include "Vec2/Vec2.h"

namespace flx {
	struct mat22 {
		float a11, a12, a21, a22;

		constexpr mat22() noexcept : a11(0), a12(0), a21(0), a22(0) {}

		constexpr mat22(float a11, float a12, float a21, float a22) noexcept : a11(a11), a12(a12), a21(a21), a22(a22) {}

		constexpr mat22(const mat22&) = default;

		explicit mat22(float rotation) noexcept {
			const float _cos = std::cos(rotation);
			const float _sin = std::sin(rotation);
			a11 = _cos;
			a12 = -_sin;
			a21 = _sin;
			a22 = _cos;
		}

		constexpr mat22 operator+(const mat22& right) const noexcept {
			return {a11 + right.a11, a12 + right.a12, a21 + right.a21, a22 + right.a22};
		}

		constexpr mat22 operator-(const mat22& right) const noexcept {
			return {a11 - right.a11, a12 - right.a12, a21 - right.a21, a22 - right.a22};
		}

		constexpr mat22 operator*(float scalar) const noexcept {
			return {a11 * scalar, a12 * scalar, a21 * scalar, a22 * scalar};
		}

		constexpr vec2 operator*(const vec2& vec) const noexcept {
			return {a11 * vec.x + a12 * vec.y, a21 * vec.x + a22 * vec.y};
		}

		constexpr mat22 operator*(const mat22& right) const noexcept {
			return {
				a11 * right.a11 + a12 * right.a21, a11 * right.a12 + a12 * right.a22,
				a21 * right.a11 + a22 * right.a21, a21 * right.a12 + a22 * right.a22};
		}

		// transpose
		constexpr mat22 tr() const noexcept {
			return {a11, a21, a12, a22};
		}

		constexpr mat22& operator*=(float scalar) noexcept {
			a11 *= scalar;
			a12 *= scalar;
			a21 *= scalar;
			a22 *= scalar;
			return *this;
		}

		constexpr mat22& operator+=(const mat22& right) noexcept {
			a11 += right.a11;
			a12 += right.a12;
			a21 += right.a21;
			a22 += right.a22;
			return *this;
		}

		constexpr mat22& operator-=(const mat22& right) noexcept {
			a11 -= right.a11;
			a12 -= right.a12;
			a21 -= right.a21;
			a22 -= right.a22;
			return *this;
		}

		constexpr bool operator==(const mat22& right) const noexcept {
			return a11 == right.a11 && a12 == right.a12 && a21 == right.a21 && a22 == right.a22;
		}

		constexpr bool operator!=(const mat22& right) const noexcept {
			return !(*this == right);
		}

		constexpr mat22 abs() const {
			return {math::abs(a11), math::abs(a12), math::abs(a21), math::abs(a22)};
		}

		static constexpr mat22 rotRight() {
			return {0, 1, -1, 0};
		}

		static constexpr mat22 rotLeft() {
			return {0, -1, 1, 0};
		}

		static constexpr mat22 rotBack() {
			return {-1, 0, 0, -1};
		}
	};
} // namespace flx

#endif