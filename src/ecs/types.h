#pragma once
#ifndef MYECS_TYPES_H
#define MYECS_TYPES_H

#include <format>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

#undef max
#define MYECS_NODISCARD [[nodiscard]]

namespace myecs {

// 1-debug, 0-release
#ifdef MYECS_DEBUG_LEVEL
inline constexpr int myecs_debug_level = MYECS_DEBUG_LEVEL;
#elif _DEBUG
inline constexpr int myecs_debug_level = 1;
#else
inline constexpr int myecs_debug_level = 0;
#endif

template <class Alloc, class T>
using rebind_alloc = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;

using u32 = ::std::uint32_t;
using u64 = ::std::uint64_t;
inline constexpr u32 u32_max = ::std::numeric_limits<u32>::max();
inline constexpr u64 u64_max = ::std::numeric_limits<u64>::max();

struct entity {
	union {
		u64 _entity;

		struct {
			u32 id;
			u32 version;
		};
	};

	constexpr entity() : entity(u32_max, u32_max) {};

	constexpr entity(const entity& other) : _entity(other._entity) {
	}

	constexpr explicit entity(u64 _entity) : _entity(_entity) {
	}

	constexpr entity(u32 id, u32 version) : id(id), version(version) {
	}

	constexpr bool operator==(const entity& other) const {
		return _entity == other._entity;
	}

	entity& operator=(const entity& other) {
		_entity = other._entity;
		return *this;
	}

	constexpr u64 get_id() const {
		return static_cast<u64>(id);
	}

	std::string string() const {
		if (is_null())
			return "null";
		return "id:" + std::to_string(id) + "/v:" + std::to_string(version);
	}

	constexpr bool is_null() const {
		return id == u32_max && version == u32_max;
	}

	// constexpr explicit operator bool() const {
	// 	return !is_null();
	// }
};

namespace types {
template <typename Type>
[[nodiscard]] inline constexpr std::string_view type_name() noexcept {
	std::string_view pretty_function{static_cast<const char*>(__FUNCSIG__)};
	u64 pos = pretty_function.find('<');
	pos = pretty_function.find('<', pos + 1);
	pos = pretty_function.find('<', pos + 1);
	auto first = pretty_function.find_first_not_of(' ', pos + 1);
	auto last = pretty_function.find_last_of('>');
	return pretty_function.substr(first, last - first);
}

template <typename Type>
[[nodiscard]] inline constexpr u64 type_hash() noexcept {
	constexpr std::string_view name = type_name<Type>();
	u64 hash = 0xCBF29CE484222325;
	for (auto c : name) {
		hash ^= c;
		hash *= 0x100000001B3;
	}
	return hash;
}

static_assert(sizeof(u64) == 8, "u64 is not 64bit!");

inline u64 rot64(u64 x, u64 k) {
	return (x >> k) | (x << (64 - k));
}

inline u64 random_map(u64 x) {
	u64 val = static_cast<u64>(x);
	val ^= 0xDEADBEEFCAFEBABE;
	val *= 0x9E3779B97F4A7C15;
	val = rot64(val, 17);
	val ^= 0x8BADF00D12345678;
	val = rot64(val, 23);
	val ^= 0x1357924680ABCDEF;
	return val;
}

namespace detail {
inline u64 _id_count = 0;
}

// Generates unique type id
template <class T>
[[nodiscard]] inline u64 type_id() noexcept {
	static const u64 id = random_map(detail::_id_count++);
	return id;
}

} // namespace types

template <class... Args>
[[noreturn]] void throw_format(std::string_view fmt, Args&&... args) {
	throw std::runtime_error(std::vformat(fmt, std::make_format_args(args...)));
}

template <class... Args>
void throw_if(bool cond, std::string_view fmt, Args&&... args) {
	if (cond) {
		throw_format(fmt, std::forward<Args>(args)...);
	}
}

} // namespace myecs

namespace std {
template <>
struct hash<myecs::entity> {
	::myecs::u64 operator()(myecs::entity e) const noexcept {
		return hash<::myecs::u64>()(e._entity);
	}
};
} // namespace std

#endif