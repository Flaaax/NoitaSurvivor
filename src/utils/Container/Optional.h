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
		T* ptr_{};

	public:
		using value_type = T;

		constexpr Optional() noexcept = default;

		constexpr explicit(false) Optional(std::nullopt_t) noexcept {}

		constexpr explicit(false) Optional(T& ref) noexcept
			: ptr_(std::addressof(ref)) {}

		constexpr Optional& operator=(std::nullopt_t) noexcept {
			reset();
			return *this;
		}

		constexpr Optional& operator=(T& ref) noexcept {
			ptr_ = std::addressof(ref); // 重新绑定
			return *this;
		}

		constexpr bool has_value() const noexcept {
			return ptr_ != nullptr;
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
			if (!ptr_) {
				throw std::bad_optional_access{};
			}
			return *ptr_;
		}

		constexpr T* value_ptr() const {
			return &value();
		}

		constexpr T* raw() noexcept {
			return ptr_;
		}

		constexpr void reset() noexcept {
			ptr_ = {};
		}

		constexpr T& emplace(T& ref) noexcept {
			ptr_ = std::addressof(ref);
			return *ptr_;
		}
	};
} // namespace flx