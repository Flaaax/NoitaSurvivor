#pragma once
#include "../Integers.h"

#include <concepts>
#include <limits>
#include <type_traits>

namespace flx::traits {
	template <class T, class... Ts>
	inline constexpr bool is_one_of_v = (std::is_same_v<T, Ts> || ...);

	template <class... Ts>
	inline constexpr u64 type_count_v = sizeof...(Ts);

	template <class>
	inline constexpr bool always_false_v = false;

	template <class T>
	concept enum_v = std::is_enum_v<T>;

	template <class From, class To>
	concept static_castable_to = requires {
		static_cast<To>(std::declval<From>());
	};

	template <class...>
	inline constexpr bool all_different_v = true;

	template <class T, class... Rest>
	inline constexpr bool all_different_v<T, Rest...> =
		((!std::same_as<T, Rest>) && ...) && all_different_v<Rest...>;

	// template <class... Ts>
	// concept all_different = all_different_v<Ts...>;

	template <std::integral T>
	inline constexpr u64 digits = static_cast<u64>(std::numeric_limits<T>::digits);

	inline constexpr u64 max_digits = 64ull;

	inline constexpr u64 u64_max = static_cast<u64>(-1);
} // namespace flx::traits