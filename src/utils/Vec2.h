#pragma once

#include<box2d/b2_math.h>
#include<SFML/System/Vector2.hpp>
#include<SFML/Graphics/Rect.hpp>
#include<optional>
#include<cmath>

#pragma push_macro("max")
#undef max


using fvec2 = sf::Vector2f;
using bvec2 = b2Vec2;
using vec2u = sf::Vector2u;
using frect = sf::FloatRect;

template <typename T>
concept vec2_t = requires(T t) {
	requires std::convertible_to<decltype(t.x), float>;
	requires std::convertible_to<decltype(t.y), float>;
};

namespace nmath {
	inline float c_sqrt(float x) {
		return ::std::sqrt(x);
	}

	inline bool c_isvalid(float x) {
		return ::std::isfinite(x);
	}

	inline constexpr float n_epsilon = 1e-5f;
	inline constexpr float n_epsilon_2 = 1e-10f;
	inline constexpr float n_max = FLT_MAX;

	template<class T>
	inline constexpr auto abs(T a) {
		return a >= static_cast<T>(0) ? a : -a;
	}
};


struct nvec2 {
	union {
		struct {
			float x, y;
		};
		bvec2 as_bvec2;
		fvec2 as_fvec2;
	};

	constexpr nvec2()noexcept :x(0.f), y(0.f) {}
	constexpr nvec2(float x, float y) noexcept :x(x), y(y) {}
	template<typename T> requires std::convertible_to<T, float>
	constexpr nvec2(T x, T y)noexcept :x(static_cast<float>(x)), y(static_cast<float>(y)) {}
	template<vec2_t _vec>
	constexpr nvec2(const _vec& vec)noexcept : x(static_cast<float>(vec.x)), y(static_cast<float>(vec.y)) {}

	constexpr nvec2(const nvec2& other) noexcept = default;
	constexpr nvec2(nvec2&& other) noexcept = default;
	constexpr nvec2& operator=(const nvec2& other)noexcept {
		x = other.x;
		y = other.y;
		return *this;
	}

	template<vec2_t T>
	constexpr void set(const T& other) {
		x = static_cast<float>(other.x);
		y = static_cast<float>(other.y);
	}

	constexpr ~nvec2() noexcept = default;

	operator fvec2& () noexcept { return as_fvec2; }
	operator const fvec2& ()const noexcept { return as_fvec2; }
	operator bvec2& () noexcept { return as_bvec2; }
	operator const bvec2& ()const noexcept { return as_bvec2; }

	//const fvec2& Fvec2()const noexcept {
	//	return m_fvec2;
	//}
	//const bvec2& Bvec2()const noexcept {
	//	return m_bvec2;
	//}
	//fvec2& Fvec2()noexcept {
	//	return m_fvec2;
	//}
	//bvec2& Bvec2()noexcept {
	//	return m_bvec2;
	//}

	template<vec2_t _vec>
	_vec to() const {
		return { static_cast<decltype(_vec::x)>(x), static_cast<decltype(_vec::y)>(y) };
	}

	constexpr nvec2 operator-() const noexcept { return { -x,-y }; }

	[[nodiscard]] constexpr nvec2 operator+ (const nvec2& right) const noexcept { return { x + right.x, y + right.y }; }
	[[nodiscard]] constexpr nvec2 operator- (const nvec2& right) const noexcept { return { x - right.x, y - right.y }; }
	[[nodiscard]] constexpr nvec2 operator* (float a) const noexcept { return { x * a, y * a }; }
	[[nodiscard]] constexpr nvec2 operator*(const nvec2& right)const noexcept { return{ x * right.x,y * right.y }; }
	[[nodiscard]] constexpr nvec2 operator/ (float a)const noexcept {
		float inv_a = 1.f / a;
		return { x * inv_a,y * inv_a };
	}
	[[nodiscard]] constexpr nvec2 operator/(const nvec2& right)const noexcept {
		float inv_x = 1.f / right.x;
		float inv_y = 1.f / right.y;
		return { x * inv_x, y * inv_y };
	}

	constexpr nvec2& operator+= (const nvec2& v) noexcept {
		x += v.x; y += v.y;
		return *this;
	}
	constexpr nvec2& operator-= (const nvec2& v) noexcept {
		x -= v.x; y -= v.y;
		return *this;
	}
	constexpr nvec2& operator*= (float a) noexcept {
		x *= a; y *= a;
		return *this;
	}
	constexpr nvec2& operator*=(const nvec2& right)noexcept {
		x *= right.x; y *= right.y;
		return *this;
	}
	constexpr nvec2& operator/=(float a) noexcept {
		float inv_a = 1.f / a;
		x *= inv_a; y *= inv_a;
		return *this;
	}

	constexpr bool operator == (const nvec2& right) const noexcept { return x == right.x && y == right.y; }
	constexpr bool operator != (const nvec2& right) const noexcept { return !(*this == right); }

	[[nodiscard]] nvec2 yFlip()const noexcept { return { x,-y }; }
	[[nodiscard]] float length() const noexcept { return ::nmath::c_sqrt(x * x + y * y); }
	[[nodiscard]] constexpr float lengthSquared() const noexcept { return x * x + y * y; }
	[[nodiscard]] constexpr float ratio()const noexcept { return x / y; }

	float normalize() noexcept {
		float len = length();
		if (len < nmath::n_epsilon) {
			return 0.0f;
		}
		float invLength = 1.0f / len;
		x *= invLength;
		y *= invLength;
		return len;
	}

	[[nodiscard]] nvec2 normalized()const noexcept {
		float len = length();
		if (len < nmath::n_epsilon) {
			return {};
		}
		float invLength = 1.0f / len;
		return { x * invLength,y * invLength };
	}

	[[nodiscard]] bool isValid() const noexcept { return ::nmath::c_isvalid(x) && ::nmath::c_isvalid(y); }

	[[nodiscard]] constexpr bool isNonNegative()const noexcept { return x > 0 && y > 0; }

	[[nodiscard]] constexpr nvec2 abs()const noexcept {
		return { nmath::abs(x), nmath::abs(y) };
	}

	[[nodiscard]] constexpr bool isZero()const noexcept {
		return nmath::abs(x) <= nmath::n_epsilon && nmath::abs(y) <= nmath::n_epsilon;
	}

	[[nodiscard]] float rad()const noexcept {
		return std::atan2(y, x);
	}

	[[nodiscard]] nvec2 rotated(float rad, nvec2 center = {}) const noexcept {
		float dx = x - center.x;
		float dy = y - center.y;
		float c = std::cos(rad);
		float s = std::sin(rad);
		return {
			center.x + dx * c - dy * s,
			center.y + dx * s + dy * c
		};
	}

};

[[nodiscard]] inline constexpr nvec2 operator* (float a, const nvec2& v) noexcept {
	return { a * v.x, a * v.y };
}
[[nodiscard]] inline constexpr nvec2 operator/ (float a, const nvec2& v) noexcept {
	float inv_x = 1.f / v.x;
	float inv_y = 1.f / v.y;
	return { a * inv_x,a * inv_y };
}

struct nrect {
	using rect_type = size_t;
	enum : rect_type {
		CenterRect = 0
	};

	union {
		struct {
			float x, y, w, h;
		};
		struct {
			nvec2 position;
			nvec2 size;
		};
		frect as_frect;
	};

	constexpr nrect() : x(0), y(0), w(0), h(0) {}
	constexpr nrect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
	constexpr nrect(float cx, float cy, float w, float h, [[maybe_unused]] rect_type)
		: x(cx - w / 2.f), y(cy - h / 2.f), w(w), h(h) {
	}
	constexpr nrect(const nvec2& pos, const nvec2& _size)
		: x(pos.x), y(pos.y), w(_size.x), h(_size.y) {
	}
	constexpr nrect(const nvec2& center, const nvec2& _size, int)
		: x(center.x - _size.x / 2), y(center.y - _size.y / 2), w(_size.x), h(_size.y) {
	}
	constexpr nrect(const frect& rect)
		: x(rect.left), y(rect.top), w(rect.width), h(rect.height) {
	}
	constexpr nrect(const nrect& other) = default;

	constexpr bool operator==(nrect other) const {
		return position == other.position && size == other.size;
	}

	constexpr nrect& operator=(nrect other) {
		position = other.position;
		size = other.size;
		return *this;
	}

	constexpr nrect& setCenter(nvec2 center) {
		x = center.x - w / 2.f;
		y = center.y - h / 2.f;
		return *this;
	}

	constexpr nrect& offset(nvec2 off) {
		x += off.x;
		y += off.y;
		return *this;
	}

	constexpr nvec2 center() const { return { x + w / 2.f, y + h / 2.f }; }
	constexpr nvec2 topLeft() const { return { x, y }; }
	constexpr nvec2 topRight() const { return { x + w, y }; }
	constexpr void setTopRight(const nvec2& tr) { y = tr.y; x = tr.x - w; }
	constexpr nvec2 bottomLeft() const { return { x, y + h }; }
	constexpr nvec2 bottomRight() const { return { x + w, y + h }; }

	constexpr bool contains(nvec2 vec) const {
		return x <= vec.x && x + w >= vec.x
			&& y <= vec.y && y + h >= vec.y;
	}

	constexpr bool overlaps(nrect other) const {
		return x < other.x + other.w && x + w > other.x
			&& y < other.y + other.h && y + h > other.y;
	}

	//Returns the intersection
	constexpr std::optional<nrect> getOverlap(nrect other) const {
		float x1 = std::max(x, other.x);
		float y1 = std::max(y, other.y);
		float x2 = std::min(x + w, other.x + other.w);
		float y2 = std::min(y + h, other.y + other.h);

		if (x1 < x2 && y1 < y2) {
			return nrect{ x1, y1, x2 - x1, y2 - y1 };
		}
		return {};
	}

	//increase width and height but remain the center
	constexpr nrect getExpand(nvec2 outLine)const {
		return nrect(center(), size + outLine * 2.f, CenterRect);
	}

	operator frect& () {
		return as_frect;
	}
	operator const frect& ()const {
		return as_frect;
	}
};


#pragma pop_macro("max")