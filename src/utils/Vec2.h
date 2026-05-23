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
		T x{};
		T y{};

		constexpr vec2_base() noexcept = default;

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

struct nvec2 : Util::internal::vec2_base<nvec2, float> {
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
	nvec2 position{};
	nvec2 size{};

	constexpr nrect() = default;

	constexpr nrect(float x, float y, float w, float h) : position(x, y), size(w, h) {
	}

	constexpr nrect(nvec2 pos, nvec2 size)
		: position(pos), size(size) {
	}

	constexpr static nrect fromCenter(nvec2 center, nvec2 size) {
		return {center - size / 2.f, size};
	}

	explicit(false) constexpr nrect(sf::FloatRect rect)
		: position(rect.position), size(rect.size) {
	}

	constexpr nrect(const nrect& other) = default;

	constexpr bool operator==(const nrect& other) const = default;

	constexpr nrect& operator=(nrect other) {
		position = other.position;
		size = other.size;
		return *this;
	}

	constexpr nrect& setCenter(nvec2 center) {
		position = center - size / 2.f;
		return *this;
	}

	constexpr nrect& offset(nvec2 off) {
		position += off;
		return *this;
	}

	constexpr nvec2 center() const {
		return position + size / 2.f;
	}

	constexpr nvec2 leftTop() const {
		return {left(), top()};
	}

	constexpr nvec2 rightTop() const {
		return {right(), top()};
	}

	constexpr nvec2 leftBottom() const {
		return {left(), bottom()};
	}

	constexpr nvec2 rightBottom() const {
		return {right(), bottom()};
	}

	constexpr void setRightTop(nvec2 rightTop) {
		position.y = rightTop.y;
		position.x = rightTop.x - size.x;
	}

	constexpr bool contains(nvec2 vec) const {
		return left() <= vec.x &&
			   right() >= vec.x &&
			   top() <= vec.y &&
			   bottom() >= vec.y;
	}

	constexpr float right() const {
		return position.x + size.x;
	}

	constexpr float bottom() const {
		return position.y + size.y;
	}

	constexpr float left() const {
		return position.x;
	}

	constexpr float top() const {
		return position.y;
	}

	constexpr bool overlaps(nrect other) const {
		return left() < other.right() && right() > other.left() && top() < other.bottom() && bottom() > other.top();
	}

	// Returns the intersection
	constexpr std::optional<nrect> getOverlap(nrect other) const {
		const float x1 = std::max(left(), other.left());
		const float y1 = std::max(top(), other.top());
		const float x2 = std::min(right(), other.right());
		const float y2 = std::min(bottom(), other.bottom());

		if (x1 < x2 && y1 < y2) {
			return nrect{x1, y1, x2 - x1, y2 - y1};
		}
		return {};
	}

	// increase width and height but remain the center
	constexpr nrect getExpand(nvec2 outLine) const {
		return fromCenter(center(), size + outLine * 2.f);
	}

	explicit(false) operator sf::FloatRect() const {
		return {position, size};
	}
};

struct nquad {
	nvec2 lt{}; // leftTop
	nvec2 rt{}; // rightTop
	nvec2 lb{}; // leftBottom
	nvec2 rb{}; // rightBottom

	constexpr nquad() = default;

	constexpr nquad(nvec2 leftTop, nvec2 rightTop, nvec2 leftBottom, nvec2 rightBottom)
		: lt(leftTop), rt(rightTop), lb(leftBottom), rb(rightBottom) {}

	constexpr static nquad fromRect(nrect rect) {
		const float left = rect.left();
		const float top = rect.top();
		const float right = rect.right();
		const float bottom = rect.bottom();

		return {{left, top},
				{right, top},
				{left, bottom},
				{right, bottom}};
	}

	constexpr void offset(nvec2 off) {
		lt += off;
		rt += off;
		lb += off;
		rb += off;
	}
};

#pragma pop_macro("max")