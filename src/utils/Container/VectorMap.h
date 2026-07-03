#pragma once
#include "../Macro.h"
#include "Map.h"
#include "Vector.h"

#include <optional>
#include <utility>

namespace flx {
	template <class T>
	class VectorMap {
	private:
		using Pair_ = std::pair<std::string_view, T>;

		Vector<std::pair<std::string_view, T>> content;
		StrMap<u64> map;

		template <class... Args>
		Pair_& emplace_nocheck(std::string key, Args&&... args) {
			const auto it = map.emplace(std::move(key), content.size());
			return content.emplace_back(it.first->first, T{std::forward<Args>(args)...});
		}

		void copy_from(const VectorMap& other) {
			content.reserve(other.content.size());
			for (const auto& [key, value] : other.content) {
				this->emplace_nocheck(std::string(key), value);
			}
		}

	public:
		VectorMap() = default;

		VectorMap(const VectorMap& other) {
			this->copy_from(other);
		}

		VectorMap(VectorMap&&) = default;

		VectorMap& operator=(const VectorMap& other) {
			if (this == &other) {
				return *this;
			}

			content.clear();
			map.clear();
			this->copy_from(other);
			return *this;
		}

		VectorMap& operator=(VectorMap&&) = default;

		template <class... Args>
		std::pair<Pair_&, bool> try_emplace(std::string_view key, Args&&... args) {
			if (const auto i = map.try_find(key)) {
				return {content[*i], false};
			}

			return {this->emplace_nocheck(std::string(key), FLX_FORWARD), true};
		}

		template <class... Args>
		Pair_& emplace(std::string key, Args&&... args) {
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

		const T& operator[](std::string_view key) const {
			return at(key);
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
