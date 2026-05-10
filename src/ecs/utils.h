#pragma once
#ifndef MYECS_UTILS_H
#define MYECS_UTILS_H
#include<type_traits>
#include<format>
#include<source_location>
#include "types.h"

#undef max
#undef min


namespace myecs {
	//stolen from entt
	[[nodiscard]] constexpr u64 fast_mod(u64 value, u64 mod)noexcept {
		return value & (mod - 1u);
	}

	template <typename T>
	concept Movable =
		std::is_move_assignable_v<T> &&					 // 可移动赋值
		std::is_move_constructible_v<T> &&				 // 可移动构造
		!std::is_const_v<std::remove_reference_t<T>> &&  // 非const类型
		!std::is_reference_v<T>;						 // 非引用类型

	//If a type is not movable, consider assign
	template<Movable T>
	constexpr decltype(auto) move_and_reset(T& t) noexcept {
		using RawType = std::remove_cvref_t<T>;
		if constexpr (std::is_fundamental_v<RawType>) {
			auto value = static_cast<RawType>(t);
			t = RawType{};
			return value;
		}
		else return std::move(t);
	}
}


#if defined(_DEBUG) || defined(MYECS_ENABLE_DEBUG)
#include<iostream>
namespace myecs {
	namespace internal {
		inline static void __MyAssert(bool expected, const char* msg, std::source_location location = std::source_location::current()) {
			if (!expected) {
				auto _msg = std::format("Assertion failed\nfile: {}\nline: {}\nfunction: {}\nmsg: {}\n",
										location.file_name(), location.line(), location.function_name(), msg);
				std::cerr << _msg << std::endl;;
				std::cerr << *reinterpret_cast<int*>(0);
			}
		}
	}
}
#define MYECS_ASSERT(expected, msg) ::myecs::internal::__MyAssert((expected), (msg))
#else
#define MYECS_ASSERT(expected, msg) void(0)
#endif

#endif