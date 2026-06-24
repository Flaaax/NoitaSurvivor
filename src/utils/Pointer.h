#pragma once
#include "Macro.h"

#include <memory>
#include <utility>

namespace flx {
	template <class T>
	using Shared = std::shared_ptr<T>;

	template <class T>
	using Unique = std::unique_ptr<T>;

	template <class T>
	using Weak = std::weak_ptr<T>;

	template <class T, class U = T>
	using Pair = std::pair<T, U>;

	template <class T>
	Shared<T> makeShared(T* ptr) {
		return Shared<T>(ptr);
	}

	template <class T>
	Unique<T> makeUnique(T* ptr) {
		return Unique<T>(ptr);
	}

	namespace internal {
		struct move_t {};

		template <class T>
		[[nodiscard]]
		constexpr decltype(auto) operator|(T&& value, move_t) noexcept {
			return std::move(value);
		}
	} // namespace internal

	// This is evil FR FR
	inline constexpr internal::move_t move{};

	template <class T>
	class SUnique;

	template <class T>
	class SWeak {
	private:
		T* ptr{};
		Weak<void> lifetime{};

		explicit SWeak(T* ptr, Weak<void> lifetime)
			: ptr(ptr), lifetime(lifetime | move) {}

	public:
		SWeak() = default;

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit(false) SWeak(const SUnique<U>& storage)
			: ptr(storage.storage.get()), lifetime(storage.storage) {}

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit(false) SWeak(const SWeak<U>& other)
			: ptr(other.ptr), lifetime(other.lifetime) {}

		template <class U>
		[[nodiscard]]
		SWeak<U> staticCcast() const {
			return {static_cast<U*>(ptr), lifetime};
		}

		[[nodiscard]]
		bool expired() const {
			return lifetime.expired();
		}

		[[nodiscard]]
		bool valid() const {
			return !expired();
		}

		[[nodiscard]]
		explicit operator bool() const {
			return valid();
		}

		[[nodiscard]]
		T* get() const {
			return valid() ? ptr : nullptr;
		}

		[[nodiscard]]
		T& operator*() const {
			return *ptr;
		}

		[[nodiscard]]
		T* operator->() const {
			return ptr;
		}
	};

	template <class T>
	class SUnique {
	private:
		template <class>
		friend class SWeak;

		template <class>
		friend class SUnique;

		Shared<T> storage{};

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit SUnique(Shared<U> storage)
			: storage(storage | move) {}

	public:
		SUnique() = default;

		explicit SUnique(T* ptr)
			: storage(ptr) {}

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit SUnique(Unique<U>&& ptr)
			: storage(ptr | move) {}

		template <class U = T, class... Args>
		[[nodiscard]]
		static SUnique make(Args&&... args) {
			return SUnique{std::make_shared<U>(FLX_FORWARD)};
		}

		SUnique(SUnique&&) noexcept = default;
		SUnique& operator=(SUnique&&) noexcept = default;

		SUnique(const SUnique&) = delete;
		SUnique& operator=(const SUnique&) = delete;

		[[nodiscard]]
		T* get() const {
			return storage.get();
		}

		[[nodiscard]]
		T& operator*() const {
			return *storage;
		}

		[[nodiscard]]
		T* operator->() const {
			return storage.get();
		}

		[[nodiscard]]
		explicit operator bool() const {
			return static_cast<bool>(storage);
		}

		void reset() {
			storage.reset();
		}

		template <class U = T>
			requires std::convertible_to<T*, U*>
		[[nodiscard]]
		SWeak<U> ref() const {
			return {*this};
		}
	};

	template <class T, class... Args>
	[[nodiscard]]
	SUnique<T> makeSUnique(Args&&... args) {
		return SUnique<T>::make(FLX_FORWARD);
	}
} // namespace flx