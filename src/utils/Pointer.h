#pragma once
#include <array>
#include <memory>
#include <utility>

template <class T>
using n_shared = std::shared_ptr<T>;

template <class T>
using n_unique = std::unique_ptr<T>;

template <class T>
using n_weak = std::weak_ptr<T>;

template <class T>
using n_pair = std::pair<T, T>;

namespace Util {
	template <class T>
	n_shared<T> makeShared(T* ptr) {
		return n_shared<T>(ptr);
	}

	template <class T>
	n_unique<T> makeUnique(T* ptr) {
		return n_unique<T>(ptr);
	}
} // namespace Util

namespace Util {
	using std::size_t;

	namespace internal {
		struct move_t {
			template <class T>
			[[nodiscard]]
			friend constexpr decltype(auto) operator|(T&& value, move_t) noexcept {
				return std::move(value);
			}
		};
	} // namespace internal

	// This is evil FR FR
	inline constexpr internal::move_t move{};

#if false

	namespace internal {
		template <class T>
		struct ControlBlock {
			T* ptr;
			size_t weakCount;

			explicit ControlBlock(T* p) : ptr(p), weakCount(0) {
			}
		};
	} // namespace internal

	template <typename T>
	class Weak;

	// Single-threaded unique pointer, supports weak reference.
	template <typename T>
	class Unique {
	private:
		using ControlBlock = internal::ControlBlock<T>;
		ControlBlock* control;

		void destroy() noexcept {
			if (!control)
				return;
			if (control->ptr) {
				delete control->ptr;
				control->ptr = {};
			}
			if (control->weakCount == 0) {
				delete control;
			}
			control = {};
		}

	public:
		friend class Weak<T>;

		explicit Unique() noexcept : control(nullptr) {
		}

		explicit Unique(T* ptr) : control(ptr ? new ControlBlock(ptr) : nullptr) {
		}

		~Unique() {
			destroy();
		}

		Unique(const Unique&) = delete;
		Unique& operator=(const Unique&) = delete;

		Unique(Unique&& other) noexcept : control(other.control) {
			other.control = nullptr;
		}

		Unique& operator=(Unique&& other) noexcept {
			if (this != &other) {
				destroy();
				control = other.control;
				other.control = nullptr;
			}

			return *this;
		}

		[[nodiscard]] T* get() const noexcept {
			return control ? control->ptr : nullptr;
		}

		[[nodiscard]] T* getSafely() const {
			if (!control || !control->ptr) {
				throw std::logic_error("Util::Unique::require() called on null pointer");
			}
			return control->ptr;
		}

		[[nodiscard]] T& operator*() const {
			return *get();
		}

		[[nodiscard]] T* operator->() const noexcept {
			return get();
		}

		[[nodiscard]] explicit operator bool() const noexcept {
			return get();
		}

		void reset(T* ptr = {}) {
			if (ptr == get())
				return;

			destroy();

			if (ptr) {
				control = new ControlBlock(ptr);
			}
		}

		[[nodiscard]] T* release() noexcept {
			if (!control)
				return {};
			T* raw = control->ptr;
			control->ptr = {};
			if (control->weakCount == 0) {
				delete control;
			}
			control = {};
			return raw;
		}

		void swap(Unique& other) noexcept {
			std::swap(control, other.control);
		}

		[[nodiscard]] size_t weakCount() const noexcept {
			return control ? control->weakCount : 0;
		}
	};

	template <typename T>
	class Weak {
	private:
		internal::ControlBlock<T>* control;

		void addRef() noexcept {
			if (control) {
				++control->weakCount;
			}
		}

		void releaseRef() noexcept {
			if (!control)
				return;

			if (control->weakCount > 0) {
				--control->weakCount;
			}

			if (!control->ptr && control->weakCount == 0) {
				delete control;
			}

			control = {};
		}

	public:
		Weak() noexcept : control(nullptr) {
		}

		explicit(false) Weak(const Unique<T>& unique) noexcept : control(unique.control) {
			addRef();
		}

		~Weak() {
			releaseRef();
		}

		Weak(const Weak& other) noexcept : control(other.control) {
			addRef();
		}

		Weak& operator=(const Weak& other) noexcept {
			if (this != &other) {
				releaseRef();
				control = other.control;
				addRef();
			}

			return *this;
		}

		Weak(Weak&& other) noexcept : control(other.control) {
			other.control = {};
		}

		Weak& operator=(Weak&& other) noexcept {
			if (this != &other) {
				releaseRef();
				control = other.control;
				other.control = {};
			}

			return *this;
		}

		Weak& operator=(const Unique<T>& unique) noexcept {
			releaseRef();
			control = unique.control;
			addRef();
			return *this;
		}

		bool operator==(const Weak& other) const noexcept {
			return control == other.control;
		}

		[[nodiscard]] T* get() const noexcept {
			return control ? control->ptr : nullptr;
		}

		[[nodiscard]] T* getSafely() const {
			if (!control || !control->ptr) {
				throw std::logic_error("Util::Unique::require() called on null pointer");
			}
			return control->ptr;
		}

		[[nodiscard]] bool expired() const noexcept {
			return !get();
		}

		[[nodiscard]] explicit operator T*() const noexcept {
			return get();
		}

		[[nodiscard]] T* operator->() const noexcept {
			return get();
		}

		[[nodiscard]] T& operator*() const {
			return *get();
		}

		[[nodiscard]] explicit operator bool() const noexcept {
			return get() != nullptr;
		}

		[[nodiscard]] size_t weakCount() const noexcept {
			return control ? control->weakCount : 0;
		}
	};

#endif

} // namespace Util