#pragma once
#ifndef NOITASURVIVOR_MATH_H
#define NOITASURVIVOR_MATH_H
#include"Vec2.h"
#include<SFML/Graphics/Transform.hpp>

namespace nmath {
	struct nmat22 {
		union {
			struct {
				float a11, a12, a21, a22;
			};
			nrect rect;
			/*struct {
				nvec2 row1;
				nvec2 row2;
			};*/
		};

		constexpr nmat22()noexcept :a11(0), a12(0), a21(0), a22(0) {}
		constexpr nmat22(float a11, float a12, float a21, float a22)noexcept :a11(a11), a12(a12), a21(a21), a22(a22) {}
		constexpr nmat22(const nmat22&) = default;
		nmat22(float rotation)noexcept {
			float _cos = std::cos(rotation);
			float _sin = std::sin(rotation);
			a11 = _cos;
			a12 = -_sin;
			a21 = _sin;
			a22 = _cos;
		}

		constexpr nmat22 operator+(const nmat22& right) const noexcept {
			return { a11 + right.a11, a12 + right.a12, a21 + right.a21, a22 + right.a22 };
		}

		constexpr nmat22 operator-(const nmat22& right) const noexcept {
			return { a11 - right.a11, a12 - right.a12, a21 - right.a21, a22 - right.a22 };
		}

		constexpr nmat22 operator*(float scalar) const noexcept {
			return { a11 * scalar, a12 * scalar, a21 * scalar, a22 * scalar };
		}

		constexpr nvec2 operator*(const nvec2& vec) const noexcept {
			return { a11 * vec.x + a12 * vec.y, a21 * vec.x + a22 * vec.y };
		}

		constexpr nmat22 operator*(const nmat22& right) const noexcept {
			return {
				a11 * right.a11 + a12 * right.a21, a11 * right.a12 + a12 * right.a22,
				a21 * right.a11 + a22 * right.a21, a21 * right.a12 + a22 * right.a22
			};
		}

		//transpose
		constexpr nmat22 tr() const noexcept {
			return { a11, a21, a12, a22 };
		}

		constexpr nmat22& operator*=(float scalar) noexcept {
			a11 *= scalar; a12 *= scalar;
			a21 *= scalar; a22 *= scalar;
			return *this;
		}

		constexpr nmat22& operator+=(const nmat22& right) noexcept {
			a11 += right.a11; a12 += right.a12;
			a21 += right.a21; a22 += right.a22;
			return *this;
		}

		constexpr nmat22& operator-=(const nmat22& right) noexcept {
			a11 -= right.a11; a12 -= right.a12;
			a21 -= right.a21; a22 -= right.a22;
			return *this;
		}

		constexpr bool operator==(const nmat22& right) const noexcept {
			return a11 == right.a11 && a12 == right.a12 && a21 == right.a21 && a22 == right.a22;
		}

		constexpr bool operator!=(const nmat22& right) const noexcept {
			return !(*this == right);
		}

		constexpr nmat22 abs()const {
			return { nmath::abs(a11),nmath::abs(a12),nmath::abs(a21),nmath::abs(a22) };
		}
	};

	static constexpr nmat22 ROT_RIGHT = { 0, 1, -1, 0 };
	static constexpr nmat22 ROT_LEFT = { 0,-1,1,0 };
	static constexpr nmat22 ROT_180 = { -1,0,0,-1 };
}

using nmath::nmat22;


#endif