#pragma once
#include "../Integers.h"
#include "../Macro.h"

namespace flx::traits {
	template <class... Args>
	struct TypeArray;

	template <>
	struct TypeArray<> {
		FLX_CONSTEXPR u64 size = 0;
	};

	template <class T, class... Args>
	struct TypeArray<T, Args...> {
		FLX_CONSTEXPR u64 size = sizeof...(Args);

		template <u64 N>
			requires(N < size)
		using at = std::tuple_element_t<N, std::tuple<Args...>>;

		using begin = T;
	};
} // namespace flx::traits