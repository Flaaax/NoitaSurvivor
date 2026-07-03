#pragma once
#include <optional>

namespace flx {
	template <class T>
	class Optional : public std::optional<T> {
	public:
		using Base = std::optional<T>;
		using Base::Base;
	};

	template <class T>
	class Optional<T&> {
	private:
		T* ptr{};

	public:
		using value_type = T;

		constexpr Optional() noexcept = default;

		constexpr explicit(false) Optional(std::nullopt_t) noexcept {}

		constexpr explicit(false) Optional(T& ref) noexcept
			: ptr(std::addressof(ref)) {}

		constexpr Optional& operator=(std::nullopt_t) noexcept {
			reset();
			return *this;
		}

		constexpr Optional& operator=(T& ref) noexcept {
			ptr = std::addressof(ref); // 重新绑定
			return *this;
		}

		constexpr bool has_value() const noexcept {
			return ptr != nullptr;
		}

		constexpr explicit operator bool() const noexcept {
			return has_value();
		}

		constexpr T& operator*() const {
			return value();
		}

		constexpr T* operator->() const {
			return &value();
		}

		constexpr T& value() const {
			if (!ptr) {
				throw std::bad_optional_access{};
			}
			return *ptr;
		}

		constexpr T* raw() noexcept {
			return ptr;
		}

		constexpr void reset() noexcept {
			ptr = {};
		}

		constexpr T& emplace(T& ref) noexcept {
			ptr = std::addressof(ref);
			return *ptr;
		}
	};
} // namespace flx