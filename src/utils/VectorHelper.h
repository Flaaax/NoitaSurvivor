#pragma once
#include<type_traits>
#include<vector>

namespace Util {
	//std::vector

	// Container must support size().
	template<typename Container>
	[[nodiscard]] static bool isInRange(const Container& cont, size_t index) {
		return index < cont.size();
	}

	// Container must support indexing and pop_back().
	template<typename Container>
	static void swapErase(Container& cont, size_t index) {
		if (index >= cont.size()) return;
		std::swap(cont[index], cont.back());
		cont.pop_back();
	}

	// Generic value lookup.
	template<typename Container, typename T>
	[[nodiscard]] static bool contains(const Container& cont, const T& val) {
		return std::find(cont.begin(), cont.end(), val) != cont.end();
	}

	// Remove first matching value.
	template<typename Container, typename T>
	static bool eraseVal(Container& cont, const T& val) {
		auto it = std::find(cont.begin(), cont.end(), val);
		if (it == cont.end()) return false;
		cont.erase(it);
		return true;
	}

	// Remove all matching values.
	template<typename Container, typename T>
	static bool eraseAllVal(Container& cont, const T& val) {
		auto new_end = std::remove_if(cont.begin(), cont.end(),
									  [&](const auto& element) { return element == val; });

		const bool changed = (new_end != cont.end());
		cont.erase(new_end, cont.end());
		return changed;
	}

	template<typename Container, typename Func>
	static bool eraseIf(Container& cont, Func&& func) {
		auto new_end = std::remove_if(cont.begin(), cont.end(), std::forward<Func>(func));
		const bool changed = (new_end != cont.end());
		cont.erase(new_end, cont.end());
		return changed;
	}

	template<class T>
		requires std::is_array_v<T> && (std::is_arithmetic_v<std::remove_extent_t<T>> || std::is_enum_v<std::remove_extent_t<T>>)
	static void clearArray(T& arr) {
		std::memset(arr, 0, sizeof(arr));
	}

	template<class Map, class Key, class FuncIt>
	static bool find_key_if(Map& map, const Key& key, FuncIt&& func) {
		if (auto it = map.find(key); it != map.end()) {
			func(it->second);
			return true;
		}
		return false;
	}

	//This will clone the value
	template<class Map, class Key>
	static Map::mapped_type value_or(Map& map, const Key& key, auto&& defaultVal) {
		if (auto it = map.find(key); it != map.end()) {
			return it->second;
		}
		return Map::mapped_type(std::forward<decltype(defaultVal)>(defaultVal));
	}
}