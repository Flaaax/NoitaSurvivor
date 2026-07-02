#pragma once
#ifndef CONTAINER_H
#define CONTAINER_H
#include "types.h"
#include "utils.h"
#include <vector>

namespace myecs {
	template <class T, class Alloc = std::allocator<void>>
		requires(!std::is_same_v<T, bool>)
	class Vector : public std::vector<T, rebind_alloc<Alloc, T>> {
		using Base = std::vector<T, rebind_alloc<Alloc, T>>;
		using Base::Base;

	public:
		template <std::integral I>
		T& operator[](I i) {
			return Base::operator[](static_cast<u64>(i));
		}

		template <std::integral I>
		const T& operator[](I i) const {
			return Base::operator[](static_cast<u64>(i));
		}

		template <std::integral I>
		T& force_get(I i, T default_value = {}) {
			u64 i1 = static_cast<u64>(i);
			if (i1 >= Base::size()) {
				Base::resize(i1 + 1ull, default_value);
			}

			return this->operator[](i);
		}
	};

	template <class Alloc>
	class IdGen {
	private:
		static constexpr u32 invalid_id = u32_max;

		struct Node {
			bool valid = false;
			u32 version = 0u;
		};

		Vector<u32, rebind_alloc<Alloc, u32>> unused_id;
		Vector<Node, rebind_alloc<Alloc, Node>> sparse;
		u64 m_count = 0;

	public:
		IdGen() {
		}

		IdGen(IdGen&& other) noexcept : unused_id(std::move(other.unused_id)),
										sparse(std::move(other.sparse)),
										m_count(auto_move(other.m_count)) {
		}

		IdGen& operator=(IdGen&& other) noexcept {
			unused_id = std::move(other.unused_id);
			sparse = std::move(other.sparse);
			m_count = auto_move(other.m_count);
			return *this;
		}

		entity get() {
			if (unused_id.empty()) {
				sparse.emplace_back(Node{true, 0u});
				return entity(static_cast<u32>(m_count++), 0u);
			}
			u32 new_id = unused_id.back();
			unused_id.pop_back();
			m_count++;
			sparse[new_id].valid = true;
			return entity(new_id, sparse[new_id].version);
		}

		void ret(entity e) {
			if (active(e)) {
				++sparse[e.id].version;
				sparse[e.id].valid = false;
				unused_id.push_back(e.id);
				m_count--;
			}
		}

		bool active(entity e) const {
			return e.id < sparse.size() && sparse[e.id].valid && sparse[e.id].version == e.version;
		}

		u64 count() const {
			return m_count;
		}

		u64 max_count() const {
			return sparse.size();
		}

		void clear() {
			unused_id.clear();
			sparse.clear();
			m_count = 0;
		}

		bool full() const {
			return unused_id.empty();
		}
	};

} // namespace myecs

#endif