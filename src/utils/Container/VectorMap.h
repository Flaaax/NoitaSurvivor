#pragma once
#include "../Macro.h"
#include "Map.h"
#include "Vector.h"

#include <optional>

namespace flx {
	template <class T>
	class VectorMap {
	private:
		using Pair = std::pair<std::string_view, T>;
		Vector<std::pair<std::string_view, T>> content;
		StrMap<u64> map;

		template <class... Args>
		Pair& emplace_nocheck(std::string key, Args&&... args) {
			const auto it = map.emplace(std::move(key), content.size());
			return content.emplace_back(it.first->first, T{std::forward<Args>(args)...});
		}

	public:
		template <class... Args>
		std::pair<Pair&, bool> try_emplace(std::string_view key, Args&&... args) {
			if (const auto i = map.try_find(key)) {
				return {content[*i], false};
			}

			return {emplace_nocheck(std::string(key), FLX_FORWARD), true};
		}

		template <class... Args>
		Pair& emplace(std::string key, Args&&... args) {
			if (map.contains(key)) {
				throw std::runtime_error("Dulplicated key");
			}
			return emplace_nocheck(std::move(key), FLX_FORWARD);
		}

		bool contains(std::string_view key) const {
			return map.contains(key);
		}

		auto begin() {
			return content.begin();
		}

		auto end() {
			return content.end();
		}

		auto begin() const {
			return content.begin();
		}

		auto end() const {
			return content.end();
		}

		// Pair& at(u64 i) {
		// 	return content.at(i);
		// }
		//
		// Pair& operator[](u64 i) {
		// 	return content[i];
		// }

		T* try_get(std::string_view key) {
			if (auto i = map.try_find(key)) {
				return &content[*i].second;
			}
			return {};
		}

		T& operator[](std::string_view key) {
			return try_emplace(key).first.second;
		}

		T& at(std::string_view key) {
			return content[map.at(key)].second;
		}

		const T& at(std::string_view key) const {
			return content[map.at(key)].second;
		}

		u64 size() const {
			return content.size();
		}

		bool empty() const {
			return content.empty();
		}
	};
} // namespace flx
