#pragma once

#include <cmath>
#include <limits>
#include <optional>

namespace flx {
	namespace internal {
		template <class Vec, class T>
		concept HasPublicTXAndY = requires(Vec t) {
			{ t.x } -> std::same_as<T&>;
			{ t.y } -> std::same_as<T&>;
		};

		template <class Vec, class T>
		concept ExactlyVec2Layout =
			std::is_standard_layout_v<Vec> &&
			HasPublicTXAndY<Vec, T> &&
			sizeof(Vec) == sizeof(T) * 2 &&
			alignof(Vec) == alignof(float) &&
			offsetof(Vec, x) == 0 &&
			offsetof(Vec, y) == sizeof(T);
	} // namespace internal

	template <class Vec, class T>
	concept vec2_like = flx::internal::ExactlyVec2Layout<Vec, T>;

	namespace math {
		using std::sqrt;

		inline bool isValid(float x) {
			return std::isfinite(x);
		}

		inline constexpr float fepsilon = 1e-5f;
		inline constexpr float fepsilon2 = 1e-10f;
		inline constexpr float fmax = std::numeric_limits<float>::max();
		inline constexpr float finf = std::numeric_limits<float>::infinity();

		template <class T>
		constexpr auto abs(T a) {
			return a >= static_cast<T>(0) ? a : -a;
		}
	} // namespace math

	namespace internal {
		template <class Derived, class T>
		struct vec2_base {
			T x{};
			T y{};

			constexpr vec2_base() noexcept = default;

			constexpr vec2_base(T x, T y) noexcept : x(x), y(y) {
			}

			template <class _vec>
			explicit(!vec2_like<_vec, T>) constexpr vec2_base(const _vec& vec) noexcept
				: x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {
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
				return Derived{math::abs(x), math::abs(y)};
			}

			constexpr Derived yFlip() const noexcept {
				return Derived{x, -y};
			}

			template <class _vec>
			[[nodiscard]] constexpr _vec to() const noexcept {
				using X = std::remove_cvref_t<decltype(std::declval<_vec>().x)>;
				using Y = std::remove_cvref_t<decltype(std::declval<_vec>().y)>;

				return {static_cast<X>(x), static_cast<Y>(y)};
			}

			constexpr T dot(Derived other) const {
				return x * other.x + y * other.y;
			}

			constexpr T cross(Derived other) const {
				return x * other.y - y * other.x;
			}

			template <vec2_like<T> Vec>
			explicit(false) constexpr operator Vec() const noexcept {
				return to<Vec>();
			}
		};

		template <class Derived, class T>
		[[nodiscard]] constexpr Derived operator*(T a, const vec2_base<Derived, T>& v) noexcept {
			return {a * v.x, a * v.y};
		}

		template <class Derived, class T>
		[[nodiscard]] constexpr Derived operator/(T a, const vec2_base<Derived, T>& v) noexcept {
			const T inv_x = T(1) / v.x;
			const T inv_y = T(1) / v.y;
			return {a * inv_x, a * inv_y};
		}
	} // namespace internal

	using internal::operator*;
	using internal::operator/;

	struct vec2 : internal::vec2_base<vec2, float> {
		using Base = vec2_base;
		using Base::Base;

		template <typename T>
			requires std::convertible_to<T, float>
		constexpr vec2(T x, T y) noexcept : Base(static_cast<float>(x), static_cast<float>(y)) {
		}

		[[nodiscard]] float length() const noexcept {
			return math::sqrt(lengthSquared());
		}

		[[nodiscard]] constexpr float lengthSquared() const noexcept {
			return x * x + y * y;
		}

		[[nodiscard]] constexpr float ratio() const noexcept {
			return x / y;
		}

		float normalize() noexcept {
			const float len = length();
			if (len < math::fepsilon) {
				return 0.0f;
			}
			const float invLength = 1.0f / len;
			x *= invLength;
			y *= invLength;
			return len;
		}

		[[nodiscard]] vec2 normalized() const noexcept {
			const float len = length();
			if (len < math::fepsilon) {
				return {};
			}
			const float invLength = 1.0f / len;
			return {x * invLength, y * invLength};
		}

		[[nodiscard]] bool isValid() const noexcept {
			return math::isValid(x) && math::isValid(y);
		}

		[[nodiscard]] constexpr bool isNonNegative() const noexcept {
			return x > 0 && y > 0;
		}

		[[nodiscard]] constexpr bool isZero() const noexcept {
			return math::abs(x) <= math::fepsilon && math::abs(y) <= math::fepsilon;
		}

		[[nodiscard]] float rad() const noexcept {
			return std::atan2(y, x);
		}

		[[nodiscard]] static vec2 rad(float rad_) noexcept {
			return {std::cos(rad_), std::sin(rad_)};
		}

		[[nodiscard]] vec2 rotated(float rad, vec2 center = {}) const noexcept {
			const float dx = x - center.x;
			const float dy = y - center.y;
			const float c = std::cos(rad);
			const float s = std::sin(rad);
			return {
				center.x + dx * c - dy * s,
				center.y + dx * s + dy * c};
		}
	};

	struct vec2u : internal::vec2_base<vec2u, unsigned int> {
		using Base = vec2_base;
		using Base::Base;
	};

	namespace internal {
		template <class Rect>
		concept HasPublicRectMembers = requires(Rect t) {
			vec2_like<std::remove_cvref_t<decltype(t.position)>, float>;
			vec2_like<std::remove_cvref_t<decltype(t.size)>, float>;
		};

		template <class Rect>
		concept ExactlyFloatRectLayout =
			std::is_standard_layout_v<Rect> &&
			HasPublicRectMembers<Rect> &&
			sizeof(Rect) == sizeof(float) * 4 &&
			alignof(Rect) == alignof(float) &&
			offsetof(Rect, position) == 0 &&
			offsetof(Rect, size) == sizeof(vec2);
	} // namespace internal

	template <class Rect>
	concept rect_like = flx::internal::ExactlyFloatRectLayout<Rect>;

	struct rect {
		vec2 position{};
		vec2 size{};

		constexpr rect() = default;

		constexpr rect(float x, float y, float w, float h) : position(x, y), size(w, h) {
		}

		constexpr rect(vec2 position, vec2 size)
			: position(position), size(size) {
		}

		template <class Rect>
		explicit(!rect_like<Rect>) rect(const Rect& rect)
			: position(vec2(rect.position)), size(vec2(rect.size)) {
		}

		constexpr static rect fromCenter(vec2 center, vec2 size) {
			return {center - size / 2.f, size};
		}

		constexpr rect(const rect& other) = default;

		constexpr bool operator==(const rect& other) const = default;

		constexpr rect& operator=(rect other) {
			position = other.position;
			size = other.size;
			return *this;
		}

		constexpr rect& setCenter(vec2 center) {
			position = center - size / 2.f;
			return *this;
		}

		constexpr rect& setRight(float right) {
			position.x = right - size.x;
			return *this;
		}

		constexpr rect& setLeft(float left) {
			position.x = left;
			return *this;
		}

		constexpr rect& setXCenter(float xc) {
			position.x = xc - size.x / 2.f;
			return *this;
		}

		constexpr rect& setYCenter(float yc) {
			position.y = yc - size.y / 2.f;
			return *this;
		}

		constexpr rect& setBottom(float bottom) {
			position.y = bottom - size.y;
			return *this;
		}

		constexpr rect& offset(vec2 off) {
			position += off;
			return *this;
		}

		constexpr vec2 center() const {
			return position + size / 2.f;
		}

		constexpr vec2 leftTop() const {
			return {left(), top()};
		}

		constexpr vec2 rightTop() const {
			return {right(), top()};
		}

		constexpr vec2 leftBottom() const {
			return {left(), bottom()};
		}

		constexpr vec2 rightBottom() const {
			return {right(), bottom()};
		}

		constexpr void setRightTop(vec2 rightTop) {
			position.y = rightTop.y;
			position.x = rightTop.x - size.x;
		}

		constexpr bool contains(vec2 vec) const {
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

		constexpr bool overlaps(rect other) const {
			return left() < other.right() && right() > other.left() && top() < other.bottom() && bottom() > other.top();
		}

		// Returns the intersection
		constexpr std::optional<rect> getOverlap(rect other) const {
			const float x1 = std::max(left(), other.left());
			const float y1 = std::max(top(), other.top());
			const float x2 = std::min(right(), other.right());
			const float y2 = std::min(bottom(), other.bottom());

			if (x1 < x2 && y1 < y2) {
				return rect{x1, y1, x2 - x1, y2 - y1};
			}
			return {};
		}

		// increase width and height but remain the center
		constexpr rect getExpand(vec2 outLine) const {
			return fromCenter(center(), size + outLine * 2.f);
		}

		template <rect_like Rect>
		explicit(false) operator Rect() const {
			return {position, size};
		}
	};

	struct nquad {
		vec2 lt{}; // leftTop
		vec2 rt{}; // rightTop
		vec2 lb{}; // leftBottom
		vec2 rb{}; // rightBottom

		constexpr nquad() = default;

		constexpr nquad(vec2 leftTop, vec2 rightTop, vec2 leftBottom, vec2 rightBottom)
			: lt(leftTop), rt(rightTop), lb(leftBottom), rb(rightBottom) {}

		constexpr static nquad fromRect(rect rect) {
			const float left = rect.left();
			const float top = rect.top();
			const float right = rect.right();
			const float bottom = rect.bottom();

			return {{left, top},
					{right, top},
					{left, bottom},
					{right, bottom}};
		}

		constexpr void offset(vec2 off) {
			lt += off;
			rt += off;
			lb += off;
			rb += off;
		}
	};

} // namespace flx