#pragma once
#include <type_traits>

namespace flx::traits {
	template <class T, class... Ts>
	inline constexpr bool is_one_of_v = (std::is_same_v<T, Ts> || ...);

	template <class>
	inline constexpr bool always_false_v = false;

	template <class T>
	concept enum_v = std::is_enum_v<T>;

	template <std::integral T>
	inline constexpr u64 digits = static_cast<u64>(std::numeric_limits<T>::digits);

	inline constexpr u64 max_digits = 64ull;

	inline constexpr u64 u64_max = static_cast<u64>(-1);
} // namespace flx::traits