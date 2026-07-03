#pragma once
#include "../Container/Variant.h"
#include "../Container/Vector.h"
#include "../Container/VectorMap.h"
#include "../Exception/Expected.h"
#include "../Exception/Result.h"
#include "../Integers.h"
#include "../Pointer.h"
#include "../Traits/Traits.h"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace flx::fon {
	enum class Type : u64 {
		Null,
		Bool,
		UInt,
		Int,
		Float,
		String,
		Array,
		Object,
	};

	template <class T>
	concept ValueType = std::is_arithmetic_v<std::remove_cvref_t<T>> ||
						traits::is_one_of_v<std::remove_cvref_t<T>,
											std::string,
											std::string_view,
											std::filesystem::path, std::nullptr_t>;

	// 保持键有序
	class Fon {
	private:
		using FonArray = Vector<Fon>;
		using FonObject = VectorMap<Fon>;
		using Storage = Variant<
			std::monostate,
			bool,
			u64,
			i64,
			double,
			Unique<std::string>,
			Unique<FonArray>,
			Unique<FonObject>>;
		using const_iterator = FonArray::const_iterator;
		using iterator = FonArray::iterator;
		class Parser;

		Storage storage{};

		explicit Fon(Storage&& storage) noexcept;

		static Storage copyStorage(const Storage& storage);

		void* get(Type type);
		const void* get(Type type) const;

		template <class T>
		const T& getNoCheck() const {
			return *static_cast<const T*>(get(getType()));
		}

		template <class T>
		T& getNoCheck() {
			return *static_cast<T*>(get(getType()));
		}

		void dumpTo(std::string& out, std::string_view indent, u64 depth, bool global) const;
		[[noreturn]] static void throwInternal(std::string_view msg);

		template <ValueType T>
		static auto toValue(T&& value) {
			using U = std::remove_cvref_t<T>;

			if constexpr (std::is_same_v<U, std::nullptr_t>) {
				return std::monostate{};
			} else if constexpr (std::is_same_v<U, bool>) {
				return value;
			} else if constexpr (std::is_floating_point_v<U>) {
				return static_cast<double>(value);
			} else if constexpr (std::signed_integral<U>) {
				return static_cast<i64>(value);
			} else if constexpr (std::unsigned_integral<U>) {
				return static_cast<u64>(value);
			} else if constexpr (std::is_same_v<U, std::filesystem::path>) {
				return std::make_unique<std::string>(value.string());
			} else if constexpr (traits::is_one_of_v<U, std::string, std::string_view>) {
				return std::make_unique<std::string>(std::forward<T>(value));
			} else {
				static_assert(traits::always_false_v<U>, "Type is not supported");
			}
		}

	public:
		Fon() = default;
		Fon(const Fon& other);
		Fon& operator=(const Fon& other);
		Fon(Fon&&) = default;
		Fon& operator=(Fon&&) = default;

		template <ValueType T>
		explicit(false) Fon(T&& value)
			: storage(this->toValue(std::forward<T>(value))) {}

		template <ValueType T>
		Fon& operator=(T&& value) {
			storage = this->toValue(std::forward<T>(value));
			return *this;
		}

		static Result<Fon> loadFromFile(std::filesystem::path path);
		static Result<Fon> loadFromString(std::string content);
		static Fon array();	 // empty array
		static Fon object(); // empty object

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

		const Fon& operator[](std::string_view key) const;
		Fon& operator[](std::string_view key);
		const Fon& at(std::string_view key) const;
		Fon& at(std::string_view key);

		template <class... Args>
			requires(traits::all_convertible_to_v<std::string_view, Args...>) &&
					((traits::type_count_v<Args...>) > 1)
		Fon& at(Args&&... args) {
			auto cur = this;
			((cur = &cur->at(std::string_view(std::forward<Args>(args)))), ...);
			return *cur;
		}

		template <class... Args>
			requires(traits::all_convertible_to_v<std::string_view, Args...>) &&
					((traits::type_count_v<Args...>) > 1)
		const Fon& at(Args&&... args) const {
			auto cur = this;
			((cur = &cur->at(std::string_view(std::forward<Args>(args)))), ...);
			return *cur;
		}

		Fon& at_or_emplace(std::string_view key, Fon f = {});
		bool contains(std::string_view key) const;
		const FonObject& items() const;

		const_iterator begin() const;
		const_iterator end() const;
		const Fon& operator[](u64 i) const;
		Fon& operator[](u64 i);
		const Fon& at(u64 i) const;
		Fon& at(u64 i);
		bool contains(u64 i) const;
		Fon& push(Fon f);
		void pop();

		u64 size() const;

		std::string dump() const; // uses tab
		std::string dump(u64 indentWidth) const;
		void dumpToFile(const std::filesystem::path& file) const; // uses tab
		void dumpToFile(const std::filesystem::path& file, u64 indentWidth) const;

		template <ValueType T>
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
			} else if constexpr (traits::is_one_of_v<U, std::string, std::string_view, std::filesystem::path>) {
				return Type::String;
			} else if constexpr (std::is_same_v<std::nullptr_t, U>) {
				return Type::Null;
			} else {
				static_assert(traits::always_false_v<T>, "Type is not supported");
			}
			throwInternal("Shouldn't be here...");
		}

		template <ValueType T>
		Optional<T> getIf() const {
			using U = std::remove_cvref_t<T>;

			if constexpr (std::is_arithmetic_v<U> && !std::is_same_v<U, bool>) {
				switch (getType()) {
				case Type::Int:
					return static_cast<U>(storage.get<i64>());
				case Type::UInt:
					return static_cast<U>(storage.get<u64>());
				case Type::Float:
					return static_cast<U>(storage.get<double>());
				default:
					break;
				}
			} else if constexpr (std::is_same_v<U, bool>) {
				if (const auto p = static_cast<const bool*>(get(Type::Bool))) {
					return *p;
				}
			} else if constexpr (traits::is_one_of_v<U, std::string, std::string_view, std::filesystem::path>) {
				if (const auto p = static_cast<const std::string*>(get(Type::String))) {
					return T{*p};
				}
			} else if constexpr (std::is_same_v<U, std::nullptr_t>) {
				if (is(Type::Null)) {
					return nullptr;
				}
			} else {
				static_assert(traits::always_false_v<U>, "Type is not supported");
			}

			return std::nullopt;
		}

		template <ValueType T>
		Optional<T> getIf(std::string_view key) const {
			if (contains(key)) {
				return at(key).getIf<T>();
			}
			return std::nullopt;
		}

		template <ValueType T>
		Optional<T> getIf(u64 i) const {
			if (contains(i)) {
				return at(i).getIf<T>();
			}
			return std::nullopt;
		}

		template <ValueType T, u64 N>
		Optional<Array<T, N>> getIfArray() const {
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

		template <ValueType T>
		T get() const {
			if (auto t = getIf<T>()) {
				return std::move(*t);
			}
			throwInternal("Bad fson cast");
		}

		template <ValueType T>
		T get(std::string_view key) const {
			if (!contains(key)) {
				throwInternal("Bad fson cast");
			}
			return at(key).get<T>();
		}

		template <ValueType T>
		T getOr(T defaultVal) const {
			if (auto t = getIf<T>()) {
				return std::move(*t);
			}
			return std::move(defaultVal);
		}

		template <ValueType T>
		T getOr(std::string_view key, T defaultVal = {}) const {
			if (contains(key)) {
				if (auto ret = at(key).getIf<T>()) {
					return std::move(*ret);
				}
			}
			return std::move(defaultVal);
		}
	};
} // namespace flx::fon

namespace flx {
	using Fon = fon::Fon;
}
