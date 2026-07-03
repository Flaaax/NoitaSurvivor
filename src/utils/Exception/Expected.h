#pragma once
// #include "../Container/Variant.h"
#include "../Container/Variant.h"
#include "Exceptions.h"

namespace flx {
	template <class E>
	class BadExpectedAccess : public except::Exception {
	public:
		E err;

		BadExpectedAccess() : Exception("Bad expected access: no err") {}

		explicit BadExpectedAccess(E err)
			: Exception("Bad expected access: not OK"), err(std::move(err)) {}
	};

	template <class E>
	class Unexpected {
	private:
		E e;

	public:
		explicit Unexpected(E e)
			: e(std::move(e)) {}

		constexpr const E& err() const& noexcept {
			return e;
		}

		constexpr E&& err() && noexcept {
			return std::move(e);
		}
	};

	template <class T, class E>
	class Expected {
	private:
		Variant<T, E> val;

	public:
		explicit(false) Expected(T val) noexcept
			: val(std::in_place_index<0>, std::move(val)) {}

		template <class E1>
			requires std::constructible_from<E, E1&&>
		explicit(false) Expected(Unexpected<E1>&& e) : val(std::in_place_index<1>, std::move(e).err()) {}

		bool ok() const noexcept {
			return val.template is<0>();
		}

		explicit operator bool() const {
			return ok();
		}

		const T& value() const& {
			if (!ok()) {
				throw BadExpectedAccess<E>(val.template get<1>());
			}
			return val.template get<0>();
		}

		T&& value() && {
			if (!ok()) {
				throw BadExpectedAccess<E>(std::move(val.template get<1>()));
			}
			return std::move(val.template get<0>());
		}

		const T& operator*() const& {
			return value();
		}

		T&& operator*() && {
			return std::move(*this).value();
		}

		const T* operator->() const {
			return &value();
		}

		const E& err() const& {
			if (ok()) {
				throw BadExpectedAccess<std::monostate>();
			}
			return val.template get<1>();
		}

		E&& err() && {
			if (ok()) {
				throw BadExpectedAccess<std::monostate>();
			}
			return std::move(val.template get<1>());
		}

		template <class U>
			requires std::copy_constructible<T> && std::constructible_from<T, U&&>
		T value_or(U&& defaultValue) const& {
			if (ok()) {
				return val.template get<0>();
			}
			return T(std::forward<U>(defaultValue));
		}

		template <class U>
			requires std::move_constructible<T> && std::constructible_from<T, U&&>
		T value_or(U&& defaultValue) && {
			if (ok()) {
				return std::move(val.template get<0>());
			}
			return T(std::forward<U>(defaultValue));
		}
	};
} // namespace flx