#pragma once
#ifndef CONTAINER_H
#define CONTAINER_H
#include "types.h"
#include "utils.h"
#include <assert.h>
#include <coroutine>
#include <limits>
#include <vector>

namespace myecs {
struct Index {
	u64 i{};
	template <std::integral T = u64>
	Index(T t = 0ull) : i(static_cast<u64>(t)) {
	}
	operator u64() const {
		return i;
	}
};

namespace detail {
// Warning: It does not act exactly like bool
// Wrapper class used only for std::vector, don't use it for other usage
struct BoolWrapper {
	bool value{};

	operator bool&() {
		return value;
	}
	operator const bool&() const {
		return value;
	}
	BoolWrapper& operator=(bool b) noexcept {
		value = b;
		return *this;
	}
	bool operator==(bool b) const {
		return value == b;
	}
	BoolWrapper(bool b = false) noexcept : value(b) {
	}
	BoolWrapper(const BoolWrapper&) = default;
	BoolWrapper(BoolWrapper&& other) noexcept : value(move_and_reset(other.value)) {
	}
	BoolWrapper& operator=(BoolWrapper&& other) noexcept {
		value = move_and_reset(other.value);
		return *this;
	}
};

template <class T>
using VectorType = std::conditional_t<std::is_same_v<bool, T>, BoolWrapper, T>;
} // namespace detail

template <class _T, class Alloc = std::allocator<void>>
class Vector : public std::vector<detail::VectorType<_T>, rebind_alloc<Alloc, detail::VectorType<_T>>> {
	using Base = std::vector<detail::VectorType<_T>, rebind_alloc<Alloc, detail::VectorType<_T>>>;
	using Base::Base;
	using T = std::conditional_t<std::is_same_v<detail::BoolWrapper, _T>, bool, _T>;

public:
	template <std::integral I>
	T& operator[](I i) {
		return Base::operator[](static_cast<u64>(i));
	}

	template <std::integral I>
	const T& operator[](I i) const {
		return Base::operator[](static_cast<u64>(i));
	}

	T& force_get(Index i) {
		if (i.i >= Base::size()) {
			Base::resize(i + 1, static_cast<T>(0));
		}

		return this->operator[](i.i);
	}
};

template <class T, class Alloc = std::allocator<void>>
	requires std::is_unsigned_v<T> || std::is_same_v<T, entity>
class SparseSet {
private:
	Vector<T, Alloc> dense;	   // contains the elements
	Vector<u32, Alloc> sparse; // contains the indices

	static constexpr u32 null_id = std::numeric_limits<u32>::max();

	static u32 id(T elem) {
		if constexpr (std::is_same_v<T, entity>) {
			return elem.id;
		} else
			return static_cast<u32>(elem);
	}

public:
	static constexpr u32 _max_size = 0x1'000'000;
	using const_iterator = Vector<T, Alloc>::const_iterator;

	SparseSet() {
	}
	SparseSet(SparseSet&&) = default;
	SparseSet(const SparseSet&) = default;
	SparseSet& operator=(SparseSet&&) = default;
	SparseSet& operator=(const SparseSet&) = default;

	void insert(T elem) {
		u32 i = id(elem);
		if (i >= _max_size) {
			throw std::runtime_error("Number too big!");
		}
		if (sparse.size() <= i) {
			sparse.resize(i + 1ull, null_id);
		} else if (sparse[i] != null_id) {
			return;
		}
		dense.emplace_back(elem);
		sparse[i] = static_cast<u32>(dense.size() - 1ull);
	}

	void erase(T elem) {
		u32 i = id(elem);
		if (i >= sparse.size()) {
			return;
		}
		u32 index = sparse[i];
		if (index == null_id) {
			return;
		}

		T last_elem = dense.back();
		dense[index] = last_elem;
		sparse[id(last_elem)] = index;
		dense.pop_back();

		// in case when the dense vector is empty
		sparse[i] = null_id;
	}

	void clear() {
		dense.clear();
		sparse.clear();
	}

	bool has(T elem) const {
		u32 i = id(elem);
		if (i >= sparse.size()) {
			return false;
		}
		return sparse[i] != null_id && ((!std::is_same_v<T, entity>) || dense[sparse[i]] == elem); // Check if version matches
	}

	u64 size() const {
		return dense.size();
	}

	u64 max_value_size() const {
		return sparse.size();
	}

	const_iterator begin() const {
		return dense.begin();
	}
	const_iterator end() const {
		return dense.end();
	}
};

template <class T, class Alloc = std::allocator<void>>
	requires std::is_unsigned_v<T> || std::is_same_v<T, entity>
class IdGen {
private:
	using u32 = u32;
	static constexpr u32 invalid_id = -1;

	struct Node {
		bool valid = false;
	};

	Vector<T, rebind_alloc<Alloc, T>> unused_id;
	Vector<Node, rebind_alloc<Alloc, Node>> sparse;
	u64 m_count = 0;

public:
	IdGen() {
	}
	IdGen(IdGen&& other) noexcept : unused_id(move_and_reset(other.unused_id)),
									sparse(move_and_reset(other.sparse)),
									m_count(move_and_reset(other.m_count)) {
	}
	IdGen& operator=(IdGen&& other) noexcept {
		unused_id = move_and_reset(other.unused_id);
		sparse = move_and_reset(other.sparse);
		m_count = move_and_reset(other.m_count);
		return *this;
	}

	T get() {
		if (unused_id.empty()) {
			sparse.emplace_back(Node{true});
			return m_count++;
		}
		T new_id = unused_id.back();
		unused_id.pop_back();
		m_count++;
		sparse[new_id].valid = true;
		return new_id;
	}

	void ret(T id) {
		if (active(id)) {
			sparse[id].valid = false;
			unused_id.push_back(id);
			m_count--;
		}
	}

	bool active(T id) const {
		return id < sparse.size() && sparse[id].valid;
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

template <class Alloc>
class IdGen<entity, Alloc> {
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
	IdGen(IdGen&& other) noexcept : unused_id(move_and_reset(other.unused_id)),
									sparse(move_and_reset(other.sparse)),
									m_count(move_and_reset(other.m_count)) {
	}
	IdGen& operator=(IdGen&& other) noexcept {
		unused_id = move_and_reset(other.unused_id);
		sparse = move_and_reset(other.sparse);
		m_count = move_and_reset(other.m_count);
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
		return entity(static_cast<u32>(new_id), sparse[new_id].version);
	}

	void ret(entity e) {
		if (active(e)) {
			sparse[e.id].version++;
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

// template <class T>
// struct Iterable {
// 	struct Iter {
// 		Iterable* it{};
// 		bool stop = true;
//
// 		Iter() {
// 		}
//
// 		explicit Iter(Iterable* it) : it(it), stop(it->cur.stop()) {
// 		}
//
// 		Iter& operator++() {
// 			it->cur.next();
// 			stop = it->cur.stop();
// 			return *this;
// 		}
// 		decltype(auto) operator*() const {
// 			return it->cur.get();
// 		}
// 		decltype(auto) operator*() {
// 			return it->cur.get();
// 		}
// 		bool operator==(const Iter& other) const {
// 			return stop && other.stop;
// 		}
// 	};
//
// 	T cur;
//
// 	explicit Iterable(T cur) : cur(cur) {
// 	}
//
// 	Iter begin() {
// 		return Iter(this);
// 	}
//
// 	static Iter end() {
// 		return {};
// 	}
// };

} // namespace myecs

#endif