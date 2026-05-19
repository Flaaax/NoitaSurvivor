#pragma once
#include "../Assert.h"
#include "../Pointer.h"
#include <memory>
#include <type_traits>

namespace Util {
	struct NoDelete {};

	struct StringViewHash : public std::hash<std::string_view> {
		using is_transparent = void;
	};

	template <typename T>
	using BaseStdMap = std::unordered_map<
		std::string,
		T,
		StringViewHash,
		std::equal_to<> // C++14+
		>;

	template <class T>
	class StdMap : public BaseStdMap<T> {
		using Base = BaseStdMap<T>;

	public:
		T& operator[](std::string_view key) {
			if (auto it = this->find(key); it != this->end()) {
				return it->second;
			}
			return Base::operator[](std::string(key));
		}

		T* try_find(std::string_view key) {
			const auto it = Base::find(key);
			return it != Base::end() ? &it->second : nullptr;
		}

		T value_or(std::string_view key, T defaultValue) {
			const auto it = Base::find(key);
			if (it == Base::end()) {
				return defaultValue;
			}
			return it->second;
		}
	};

	template <class T>
	struct Wrapper {
		T value;
		Wrapper() = default;

		Wrapper(const T& val) : value(val) {
		}

		Wrapper(T&& val) : value(std::move(val)) {
		}

		operator T&() {
			return value;
		}

		operator const T&() const {
			return value;
		}
	};

	using BoolWrapper = Wrapper<bool>;
} // namespace Util