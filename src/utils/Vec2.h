#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <box2d/types.h>
#include <cmath>
#include <optional>

#pragma push_macro("max")
#undef max

template <class Vec, class T>
concept vec2_t = requires(Vec t) {
	requires std::convertible_to<decltype(t.x), T>;
	requires std::convertible_to<decltype(t.y), T>;
};

namespace nmath {
	using std::sqrt;

	inline bool isValid(float x) {
		return ::std::isfinite(x);
	}

	inline constexpr float n_epsilon = 1e-5f;
	inline constexpr float n_epsilon_2 = 1e-10f;
	inline constexpr float n_max = FLT_MAX;

	template <class T>
	constexpr auto abs(T a) {
		return a >= static_cast<T>(0) ? a : -a;
	}
}; // namespace nmath

namespace Util::internal {
	template <class Derived, class T>
	struct vec2_base {
		T x, y;

		constexpr vec2_base() noexcept : x(0), y(0) {
		}

		constexpr vec2_base(T x, T y) noexcept : x(x), y(y) {
		}

		template <vec2_t<T> _vec>
		explicit(false) constexpr vec2_base(const _vec& vec) noexcept
			: x(static_cast<T>(vec.x)),
			  y(static_cast<T>(vec.y)) {
		}

		constexpr Derived operator-() const noexcept {
			return Derived{-x, -y};
		}

		constexpr Derived operator+(const Derived& right) const noexcept {
			return Derived{x + right.x, y + right.y};
		}

		constexpr Derived operator+(const T right) const noexcept {
			return Derived{x + right, y + right};
		}

		constexpr Derived operator-(const Derived& right) const noexcept {
			return Derived{x - right.x, y - right.y};
		}

		constexpr Derived operator-(const T right) const noexcept {
			return Derived{x - right, y - right};
		}

		constexpr Derived operator*(T a) const noexcept {
			return Derived{x * a, y * a};
		}

		constexpr Derived operator*(const Derived& right) const noexcept {
			return Derived{x * right.x, y * right.y};
		}

		constexpr Derived operator/(T a) const noexcept {
			T inv_a = T(1) / a;
			return Derived{x * inv_a, y * inv_a};
		}

		constexpr Derived operator/(const Derived& right) const noexcept {
			return Derived{x / right.x, y / right.y};
		}

		constexpr Derived& operator+=(const Derived& v) noexcept {
			x += v.x;
			y += v.y;
			return static_cast<Derived&>(*this);
		}

		constexpr Derived& operator-=(const Derived& v) noexcept {
			x -= v.x;
			y -= v.y;
			return static_cast<Derived&>(*this);
		}

		constexpr Derived& operator*=(T a) noexcept {
			x *= a;
			y *= a;
			return static_cast<Derived&>(*this);
		}

		constexpr bool operator==(const Derived& right) const noexcept {
			return x == right.x && y == right.y;
		}

		constexpr bool operator!=(const Derived& right) const noexcept {
			return !(*this == right);
		}

		constexpr Derived abs() const noexcept {
			return Derived{nmath::abs(x), nmath::abs(y)};
		}

		constexpr Derived yFlip() const noexcept {
			return Derived{x, -y};
		}

		template <vec2_t<T> _vec>
		[[nodiscard]] constexpr _vec to() const noexcept {
			using X = std::remove_cvref_t<decltype(std::declval<_vec>().x)>;
			using Y = std::remove_cvref_t<decltype(std::declval<_vec>().y)>;

			return {static_cast<X>(x), static_cast<Y>(y)};
		}

		T dot(Derived other) const {
			return x * other.x + y * other.y;
		}

		T cross(Derived other) const {
			return x * other.y - y * other.x;
		}
	};

	template <class Derived, class T>
	[[nodiscard]] inline constexpr Derived operator*(T a, const vec2_base<Derived, T>& v) noexcept {
		return {a * v.x, a * v.y};
	}

	template <class Derived, class T>
	[[nodiscard]] inline constexpr Derived operator/(T a, const vec2_base<Derived, T>& v) noexcept {
		const T inv_x = T(1) / v.x;
		const T inv_y = T(1) / v.y;
		return {a * inv_x, a * inv_y};
	}

} // namespace Util::internal

using Util::internal::operator*;
using Util::internal::operator/;

struct nvec2 : public Util::internal::vec2_base<nvec2, float> {
	using Base = vec2_base;
	using Base::Base;

	template <typename T>
		requires std::convertible_to<T, float>
	constexpr nvec2(T x, T y) noexcept : Base(static_cast<float>(x), static_cast<float>(y)) {
	}

	explicit(false) operator sf::Vector2f() const noexcept {
		return to<sf::Vector2f>();
	}

	explicit(false) operator b2Vec2() const noexcept {
		return to<b2Vec2>();
	}

	[[nodiscard]] float length() const noexcept {
		return ::nmath::sqrt(lengthSquared());
	}

	[[nodiscard]] constexpr float lengthSquared() const noexcept {
		return x * x + y * y;
	}

	[[nodiscard]] constexpr float ratio() const noexcept {
		return x / y;
	}

	float normalize() noexcept {
		const float len = length();
		if (len < nmath::n_epsilon) {
			return 0.0f;
		}
		const float invLength = 1.0f / len;
		x *= invLength;
		y *= invLength;
		return len;
	}

	[[nodiscard]] nvec2 normalized() const noexcept {
		const float len = length();
		if (len < nmath::n_epsilon) {
			return {};
		}
		const float invLength = 1.0f / len;
		return {x * invLength, y * invLength};
	}

	[[nodiscard]] bool isValid() const noexcept {
		return ::nmath::isValid(x) && ::nmath::isValid(y);
	}

	[[nodiscard]] constexpr bool isNonNegative() const noexcept {
		return x > 0 && y > 0;
	}

	[[nodiscard]] constexpr bool isZero() const noexcept {
		return nmath::abs(x) <= nmath::n_epsilon && nmath::abs(y) <= nmath::n_epsilon;
	}

	[[nodiscard]] float rad() const noexcept {
		return std::atan2(y, x);
	}

	[[nodiscard]] nvec2 rotated(float rad, nvec2 center = {}) const noexcept {
		const float dx = x - center.x;
		const float dy = y - center.y;
		const float c = std::cos(rad);
		const float s = std::sin(rad);
		return {
			center.x + dx * c - dy * s,
			center.y + dx * s + dy * c};
	}
};

struct nvec2u : public Util::internal::vec2_base<nvec2u, unsigned int> {
	using Base = Util::internal::vec2_base<nvec2u, unsigned int>;
	using Base::Base;

	explicit(false) operator sf::Vector2u() const noexcept {
		return to<sf::Vector2u>();
	}
};

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
	};

	constexpr nrect() : x(0), y(0), w(0), h(0) {
	}

	constexpr nrect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {
	}

	constexpr nrect(float cx, float cy, float w, float h, [[maybe_unused]] rect_type)
		: x(cx - w / 2.f), y(cy - h / 2.f), w(w), h(h) {
	}

	constexpr nrect(const nvec2& pos, const nvec2& _size)
		: x(pos.x), y(pos.y), w(_size.x), h(_size.y) {
	}

	constexpr nrect(const nvec2& center, const nvec2& _size, int)
		: x(center.x - _size.x / 2), y(center.y - _size.y / 2), w(_size.x), h(_size.y) {
	}

	explicit(false) constexpr nrect(const sf::FloatRect& rect)
		: position(rect.position), size(rect.size) {
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

	constexpr nvec2 center() const {
		return {x + w / 2.f, y + h / 2.f};
	}

	constexpr nvec2 topLeft() const {
		return {x, y};
	}

	constexpr nvec2 topRight() const {
		return {x + w, y};
	}

	constexpr void setTopRight(const nvec2& tr) {
		y = tr.y;
		x = tr.x - w;
	}

	constexpr nvec2 bottomLeft() const {
		return {x, y + h};
	}

	constexpr nvec2 bottomRight() const {
		return {x + w, y + h};
	}

	constexpr bool contains(nvec2 vec) const {
		return x <= vec.x && x + w >= vec.x && y <= vec.y && y + h >= vec.y;
	}

	constexpr bool overlaps(nrect other) const {
		return x < other.x + other.w && x + w > other.x && y < other.y + other.h && y + h > other.y;
	}

	// Returns the intersection
	constexpr std::optional<nrect> getOverlap(nrect other) const {
		const float x1 = std::max(x, other.x);
		const float y1 = std::max(y, other.y);
		const float x2 = std::min(x + w, other.x + other.w);
		const float y2 = std::min(y + h, other.y + other.h);

		if (x1 < x2 && y1 < y2) {
			return nrect{x1, y1, x2 - x1, y2 - y1};
		}
		return std::nullopt;
	}

	// increase width and height but remain the center
	constexpr nrect getExpand(nvec2 outLine) const {
		return nrect(center(), size + outLine * 2.f, CenterRect);
	}

	explicit(false) operator sf::FloatRect() const {
		return {position, size};
	}

	//
	// explicit(false) operator const sf::FloatRect&() const {
	// 	return *reinterpret_cast<const sf::FloatRect*>(this);
	// }
};

#pragma pop_macro("max")