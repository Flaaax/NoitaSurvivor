#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "src/utils/Container/Map.h"

class DebugVariables {
private:
	struct HolderBase {
		virtual ~HolderBase() = default;
	};

	template <class T>
	struct Holder final : HolderBase {
		T value{};

		T* get() noexcept {
			return &value;
		}
	};

	struct Entry {
		void* ptr{};
		const void* typeId{};
		n_unique<HolderBase> holder{};
	};

	static Util::StrMap<Entry>& vars() {
		static Util::StrMap<Entry> data;
		return data;
	}

	template <class T>
	static const void* id() noexcept {
		static constexpr char value{};
		return &value;
	}

	[[noreturn]] static void throwDuplicatedKey(std::string_view key) {
		Logger::error_and_throw("DebugVariables: duplicated key: " + std::string(key));
	}

	[[noreturn]] static void throwTypeMismatch(std::string_view key) {
		Logger::error_and_throw("DebugVariables: type mismatch for key: " + std::string(key));
	}

public:
	// 将已存在的变量注册到Debug变量中。不持有所有权。
	// 每一个key对应一个变量。不会因为T的不同而不同。
	// key重复时抛异常。
	template <class T>
	static void lend(T& var, std::string_view key) {
		using Stored = std::remove_cvref_t<T>;

		static_assert(!std::is_const_v<std::remove_reference_t<T>>, "DebugVariables::lend does not accept const variables.");
		static_assert(!std::is_volatile_v<std::remove_reference_t<T>>, "DebugVariables::lend does not accept volatile variables.");

		auto& data = vars();
		if (data.contains(key)) {
			throwDuplicatedKey(key);
		}

		data.emplace(std::string(key), Entry{
										   .ptr = static_cast<void*>(std::addressof(var)),
										   .typeId = id<Stored>(),
										   .holder = nullptr});
	}

	// 从全局访问这个Debug变量。在无key时返回null。
	// 如果有key且访问时使用的T不匹配，则抛出异常。
	template <class T>
	static T* get(std::string_view key) {
		static_assert(!std::is_reference_v<T>, "DebugVariables::get<T> requires T to be a non-reference type.");
		static_assert(!std::is_volatile_v<T>, "DebugVariables::get<T> does not accept volatile T.");

		using Stored = std::remove_cv_t<T>;

		auto& data = vars();
		const auto it = data.find(key);
		if (it == data.end()) {
			return nullptr;
		}

		if (it->second.typeId != id<Stored>()) {
			throwTypeMismatch(key);
		}

		return static_cast<T*>(it->second.ptr);
	}

	// 尝试显式创建并初始化一个Debug变量，并持有所有权。
	template <class T>
	static T& try_emplace(std::string_view key, T initVal = T{}) {
		static_assert(!std::is_const_v<T>, "DebugVariables::emplace<T> requires non-const T.");
		static_assert(!std::is_volatile_v<T>, "DebugVariables::emplace<T> requires non-volatile T.");
		static_assert(!std::is_reference_v<T>, "DebugVariables::emplace<T> requires non-reference T.");
		static_assert(std::is_default_constructible_v<T>, "DebugVariables::emplace<T> requires default constructible T.");

		auto& data = vars();
		if (data.contains(key)) {
			return *get<T>(key);
		}

		auto holder = std::make_unique<Holder<T>>();
		T* ptr = holder->get();

		data.emplace(std::string(key), Entry{
										   .ptr = static_cast<void*>(ptr),
										   .typeId = id<T>(),
										   .holder = std::move(holder)});
		*ptr = initVal;
		return *ptr;
	}
};