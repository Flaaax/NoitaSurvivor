#pragma once
#include "../Container/Map.h"
#include "../Container/Vector.h"
#include "../Integers.h"
#include "../Pointer.h"
#include "../TypeName.h"
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace flx::json {
	enum class Type {
		Null,
		Bool,
		Int,
		UInt,
		Float,
		String,
		Array,
		Object,
	};

	template <class T>
	concept JsonType = std::is_arithmetic_v<T> ||
					   type::is_one_of_v<T, std::string, std::string_view, std::filesystem::path>;

	// 读取支持常用 JSON5 语法，输出保持标准 JSON。
	class Json {
	private:
		using JsonArray = Vector<Json>;
		using JsonObject = StrMap<Json>;
		using Storage = std::variant<std::monostate, bool, i64, u64, double, Unique<std::string>, Unique<JsonArray>, Unique<JsonObject>>;
		using const_iterator = JsonArray::const_iterator;
		class Parser;

		Storage storage{};

		void* get(Type type);
		const void* get(Type type) const;

		template <class T>
		const T& getNoCheck() const {
			return *static_cast<const T*>(get(getType()));
		}

		void dumpTo(std::string& out, std::string_view indent, u64 depth) const;
		[[noreturn]] static void throwInternal(std::string_view msg);

	public:
		explicit Json() = default;
		explicit Json(Storage&& storage) noexcept;

		static Json loadFromFile(std::filesystem::path path);
		static Json loadFromString(std::string content);

		Type getType() const;
		bool is(Type type) const;

		bool isNull() const;
		bool isBool() const;
		bool isNumber() const;
		bool isInt() const;
		bool isUInt() const;
		bool isFloat() const;
		bool isArray() const;
		bool isObject() const;
		bool isString() const;

		const Json& operator[](std::string_view key) const;
		const Json& at(std::string_view key) const;

		bool contains(std::string_view key) const;
		const JsonObject& items() const;

		const_iterator begin() const;
		const_iterator end() const;
		const Json& operator[](u64 i) const;
		const Json& at(u64 i) const;
		bool contains(u64 i) const;

		u64 size() const;

		std::string dump() const;
		std::string dump(u64 indentWidth) const;

		void dumpToFile(const std::filesystem::path& file) const;
		void dumpToFile(const std::filesystem::path& file, u64 indentWidth) const;

		template <class T>
		static Type getSupportedType() {
			using U = std::remove_cvref_t<T>;

			if constexpr (std::is_floating_point_v<U>) {
				return Type::Float;
			} else if constexpr (std::is_integral_v<U> && !std::is_same_v<bool, U> && std::is_signed_v<U>) {
				return Type::Int;
			} else if constexpr (std::is_integral_v<U> && !std::is_same_v<bool, U>) {
				return Type::UInt;
			} else if constexpr (std::is_same_v<bool, U>) {
				return Type::Bool;
			} else if constexpr (type::is_one_of_v<U, std::string, std::string_view, std::filesystem::path>) {
				return Type::String;
			} else if constexpr (std::is_same_v<std::nullptr_t, U>) {
				return Type::Null;
			} else {
				static_assert(type::always_false_v<T>, "Type is not supported");
			}
			throwInternal("Shouldn't be here...");
		}

		template <class T>
		std::optional<T> getIf() const {
			using U = std::remove_cvref_t<T>;

			if constexpr (std::is_arithmetic_v<U> && !std::is_same_v<U, bool>) {
				switch (getType()) {
				case Type::Int:
					return static_cast<U>(std::get<i64>(storage));
				case Type::UInt:
					return static_cast<U>(std::get<u64>(storage));
				case Type::Float:
					return static_cast<U>(std::get<double>(storage));
				default:
					break;
				}
			} else if constexpr (std::is_same_v<U, bool>) {
				if (const auto p = static_cast<const bool*>(get(Type::Bool))) {
					return *p;
				}
			} else if constexpr (type::is_one_of_v<U, std::string, std::string_view, std::filesystem::path>) {
				if (const auto p = static_cast<const std::string*>(get(Type::String))) {
					return T{*p};
				}
			} else if constexpr (std::is_same_v<U, std::nullptr_t>) {
				if (is(Type::Null)) {
					return nullptr;
				}
			} else {
				static_assert(type::always_false_v<U>, "Type is not supported");
			}

			return std::nullopt;
		}

		template <class T>
		std::optional<T> getIf(std::string_view key) const {
			if (contains(key)) {
				return at(key).getIf<T>();
			}
			return std::nullopt;
		}

		template <class T>
		std::optional<T> getIf(u64 i) const {
			if (contains(i)) {
				return at(i).getIf<T>();
			}
			return std::nullopt;
		}

		template <class T, u64 N>
		std::optional<Array<T, N>> getIfArray() const {
			if (!isArray() || size() != N) {
				return std::nullopt;
			}
			Array<T, N> ret{};
			for (u64 i = 0; i < N; ++i) {
				if (auto v = at(i).getIf<T>()) {
					ret[i] = std::move(*v);
				} else {
					return std::nullopt;
				}
			}
			return ret;
		}

		template <class T>
		T get() const {
			if (auto t = getIf<T>()) {
				return std::move(*t);
			}
			throwInternal("Bad json cast");
		}

		template <class T>
		T get(std::string_view key) const {
			if (!contains(key)) {
				throwInternal("Bad json cast");
			}
			return at(key).get<T>();
		}

		template <class T>
		T getOr(T defaultVal) const {
			if (auto t = getIf<T>()) {
				return std::move(*t);
			}
			return std::move(defaultVal);
		}

		template <class T>
		T getOr(std::string_view key, T defaultVal = {}) const {
			if (contains(key)) {
				if (auto ret = at(key).getIf<T>()) {
					return std::move(*ret);
				}
			}
			return std::move(defaultVal);
		}
	};

}

namespace flx {
	using Json = json::Json;
}
