#pragma once
#include <memory>
#include <type_traits>

namespace flx {
	// struct NoDelete {};

	struct StringViewHash : std::hash<std::string_view> {
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
	class StrMap : public BaseStdMap<T> {
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

		const T* try_find(std::string_view key) const {
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

		T& at(std::string_view key) {
			if (auto ret = try_find(key)) {
				return *ret;
			}
			throw std::out_of_range(std::string(key));
		}
	};
} // namespace Util