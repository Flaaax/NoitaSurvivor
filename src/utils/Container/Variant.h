#pragma once
#include "Optional.h"
#include "src/utils/Traits/Traits.h"

#include <variant>

namespace flx {
	inline constexpr std::monostate monostate_v{};

	template <class... Args>
	class Variant : public std::variant<Args...> {
	public:
		using Base = std::variant<Args...>;
		using Base::Base;
		using Base::operator=;

		static constexpr bool hasEmptyState = traits::is_one_of_v<std::monostate, Args...>;

		Base& base() {
			return static_cast<Base&>(*this);
		}

		const Base& base() const {
			return static_cast<const Base&>(*this);
		}

		bool isEmpty() const
			requires hasEmptyState
		{
			return is<std::monostate>();
		}

		explicit operator bool() const
			requires hasEmptyState
		{
			return !isEmpty();
		}

		void reset()
			requires hasEmptyState
		{
			base().template emplace<std::monostate>();
		}

		template <class T>
		T& get() {
			return std::get<T>(base());
		}

		template <class T>
		const T& get() const {
			return std::get<T>(base());
		}

		template <class T>
		Optional<T&> getIf() {
			if (!is<T>()) {
				return {};
			}
			return get<T>();
		}

		template <class T>
		Optional<const T&> getIf() const {
			if (!is<T>()) {
				return {};
			}
			return get<T>();
		}

		template <class T>
		bool is() const {
			return std::holds_alternative<T>(base());
		}
	};
} // namespace flx