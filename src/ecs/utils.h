#pragma once
#ifndef MYECS_UTILS_H
#define MYECS_UTILS_H
#include "types.h"
#include <type_traits>

#undef max
#undef min

namespace myecs {
	// stolen from entt
	// value must be power of 2
	[[nodiscard]] constexpr u64 fast_mod(u64 value, u64 mod) noexcept {
		return value & (mod - 1u);
	}

	// template <typename T>
	// concept Movable =
	// 	std::is_move_assignable_v<T> &&					// Move assignable
	// 	std::is_move_constructible_v<T> &&				// Move constructible
	// 	!std::is_const_v<std::remove_reference_t<T>> && // Non-const type
	// 	!std::is_reference_v<T>;						// Non-reference type

	// If a type is not movable, consider assign
	template <class T>
		requires std::is_fundamental_v<std::remove_cvref_t<T>>
	constexpr decltype(auto) auto_move(T& t) noexcept {
		using RawType = std::remove_cvref_t<T>;
		auto value = static_cast<RawType>(t);
		t = RawType{};
		return value;
	}

	constexpr bool is_power_of_two(u64 n) {
		return n > 0 && (n & (n - 1)) == 0;
	}
} // namespace myecs

#if defined(_DEBUG) || defined(MYECS_ENABLE_DEBUG)
#include <iostream>
#include <format>
#include <source_location>

namespace myecs {
	namespace internal {
		inline static void __MyAssert(bool expected, const char* msg, std::source_location location = std::source_location::current()) {
			if (!expected) {
				auto _msg = std::format("Assertion failed\nfile: {}\nline: {}\nfunction: {}\nmsg: {}\n",
										location.file_name(), location.line(), location.function_name(), msg);
				std::cerr << _msg << std::endl;
				;
				std::cerr << *reinterpret_cast<int*>(0);
			}
		}
	} // namespace internal
} // namespace myecs

#define MYECS_ASSERT(expected, msg) ::myecs::internal::__MyAssert((expected), (msg))
#else
#define MYECS_ASSERT(expected, msg) void(0)
#endif

#endif