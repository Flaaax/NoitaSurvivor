#pragma once
#include "Macro.h"
#include "Traits/Traits.h"

#include <concepts>
#include <memory>
#include <type_traits>
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
	class EnableSWeakFromThis;

	namespace internal {
		template <class>
		struct IsSWeakFromThisBase : std::false_type {};

		template <class T>
		struct IsSWeakFromThisBase<EnableSWeakFromThis<T>> : std::true_type {};

		template <class T>
		concept hasSWeakFromThisBase = requires {
			typename T::SWeakFromThisBase;
		} && IsSWeakFromThisBase<typename T::SWeakFromThisBase>::value && std::convertible_to<T*, typename T::SWeakFromThisBase*>;

		template <class A, class B>
			requires std::equality_comparable_with<A*, B*>
		bool ownerEqual(const Weak<A>& a, const Weak<B>& b) {
			return !a.owner_before(b) && !b.owner_before(a);
		}
	} // namespace internal

	// SWeak is a non-owning observer.
	// It does not extend the lifetime of the object.
	// User must ensure no destruction happens during use.
	template <class T>
	class SWeak {
	private:
		template <class>
		friend class SWeak;
		template <class>
		friend class EnableSWeakFromThis;
		template <class>
		friend class SUnique;

		T* ptr{};
		Weak<void> lifetime{};

		explicit(false) SWeak(T* ptr, Weak<void> lifetime)
			: ptr(ptr), lifetime(lifetime | move) {}

	public:
		SWeak() = default;

		explicit(false) SWeak(std::nullptr_t) : SWeak() {}

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit(false) SWeak(const SUnique<U>& storage)
			: ptr(storage.storage.get()), lifetime(storage.storage) {}

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit(false) SWeak(const SWeak<U>& other)
			: ptr(other.ptr), lifetime(other.lifetime) {}

		template <class U>
			requires traits::static_castable_to<T*, U*>
		[[nodiscard]]
		SWeak<U> staticCast() const {
			return {static_cast<U*>(ptr), lifetime};
		}

		[[nodiscard]]
		auto constCast() const {
			using U = std::remove_const_t<T>;
			return SWeak<U>(const_cast<U*>(ptr), lifetime);
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
			return ptr;
		}

		[[nodiscard]]
		T* at() const {
			if (!valid()) {
				throw std::bad_weak_ptr();
			}
			return ptr;
		}

		[[nodiscard]]
		T& operator*() const {
			return *ptr;
		}

		[[nodiscard]]
		T* operator->() const {
			return ptr;
		}

		// template <class U>
		// 	requires std::equality_comparable_with<T*, U*>
		// bool operator==(const SWeak<U>& other) const {
		// 	return internal::ownerEqual(lifetime, other.lifetime);
		// }

		template <class U>
			requires std::equality_comparable_with<T*, U*>
		bool operator==(const SWeak<U>& other) const {
			return ptr == other.ptr && internal::ownerEqual(lifetime, other.lifetime);
		}
	};

	// Thread-unfriendly
	template <class T>
	class SUnique {
	private:
		template <class>
		friend class SWeak;

		template <class>
		friend class SUnique;

		Shared<T> storage{};

		template <class U>
		void bindSWeakFromThis(const Shared<U>& storage_) {
			if (!storage_) {
				return;
			}

			if constexpr (internal::hasSWeakFromThisBase<U>) {
				static_cast<U::SWeakFromThisBase*>(storage_.get())->lifetime = storage;
			}

			if constexpr (!std::same_as<T, U> && std::convertible_to<U*, EnableSWeakFromThis<T>*>) {
				static_cast<EnableSWeakFromThis<T>*>(storage_.get())->lifetime = storage;
			}
		}

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit SUnique(Shared<U> storage_) : storage(storage_) {
			this->bindSWeakFromThis(storage_);
		}

	public:
		SUnique() = default;

		// explicit SUnique(T* ptr)
		// 	: SUnique(Shared<T>(ptr)) {}

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit(false) SUnique(Unique<U> ptr)
			: SUnique(Shared<U>(ptr | move)) {}

		template <class U>
			requires std::convertible_to<U*, T*>
		explicit(false) SUnique(SUnique<U>&& other) noexcept
			: SUnique(other.storage | move) {}

		template <class U = T, class... Args>
		[[nodiscard]]
		static SUnique make(Args&&... args) {
			return SUnique{std::make_shared<U>(FLX_FORWARD)};
		}

		SUnique(SUnique&&) noexcept = default;
		SUnique& operator=(SUnique&&) noexcept = default;

		template <class U>
			requires std::convertible_to<U*, T*>
		SUnique& operator=(SUnique<U>&& other) noexcept {
			auto storage_ = other.storage | move;
			storage = storage_;
			bindSWeakFromThis(storage_);
			return *this;
		}

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

		template <class U>
		bool operator==(const SWeak<U>& other) const {
			return storage.get() == other.ptr;
		}
	};

	template <class T, class... Args>
	[[nodiscard]]
	SUnique<T> makeSUnique(Args&&... args) {
		return SUnique<T>::make(FLX_FORWARD);
	}

	template <class T>
	class EnableSWeakFromThis {
	private:
		template <class>
		friend class SUnique;

		Weak<void> lifetime{};

	public:
		using SWeakFromThisBase = EnableSWeakFromThis;

		EnableSWeakFromThis() = default;
		EnableSWeakFromThis(const EnableSWeakFromThis&) = delete;

		[[nodiscard]]
		SWeak<T> sweakFromThis() noexcept {
			return SWeak<T>{static_cast<T*>(this), lifetime};
		}

		[[nodiscard]]
		SWeak<const T> sweakFromThis() const noexcept {
			return {static_cast<const T*>(this), lifetime};
		}
	};
} // namespace flx
