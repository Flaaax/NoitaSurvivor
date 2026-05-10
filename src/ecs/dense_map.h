#pragma once
#ifndef MYECS_DENSE_MAP
#define MYECS_DENSE_MAP

#include"container.h"

namespace myecs {
	template<class Traits>
	class BaseMap {
	public:
		static constexpr u64 min_bucket_size = 8;
		static constexpr float load_factor = 0.875;
		static constexpr u64 expand_factor = 2;
		static constexpr u64 invalid_index = u64_max;

		inline static constexpr bool is_map = Traits::is_map;
		using Key = typename Traits::Key;
		using Val = typename Traits::Val;
		using Hash = typename Traits::Hash;
		using KeyEq = typename Traits::KeyEq;
		using Alloc = typename Traits::Alloc;

		struct Node {
			u64 prev = invalid_index;
			u64 next = invalid_index;
		};

		using Sparse = Vector<u64, Alloc>;
		using Packed = Vector<Val, Alloc>;
		using Nodes = Vector<Node, Alloc>;
		using iterator = typename Packed::iterator;
		using const_iterator = typename Packed::const_iterator;
	protected:

		Sparse sparse;			//Contains the first index of each bucket
		Packed packed;			//Contains all the elements
		Nodes nodes;			//Contains the linked list nodes
		Hash myHash;
		KeyEq myKeyeq;
		bool should_rehash;

		//Get the bucket in indexes of sparse array
		template<class _Key>
		u64 get_bucket(const _Key& key)const {
			return fast_mod(myHash(key), bucket_count());
		}

		const Key& get_key(const Val& val)const {
			if constexpr (is_map) return val.first;
			else return val;
		}

		void rehash_if_should() {
			if (should_rehash) {
				rehash(sparse.size() * expand_factor);
			}
		}

		bool update_should_rehash() {
			return should_rehash = (load_factor < ((float)size() / (float)bucket_count()));
		}

		//new_size should be 2^x
		void rehash(u64 new_size) {
			Packed oldPacked;
			oldPacked.swap(packed);
			sparse.assign(new_size, invalid_index);
			nodes.clear();		//packed and nodes are both empty
			for (auto& val : oldPacked) {
				u64 bucket = get_bucket(get_key(val));
				emplace_no_check(bucket, std::move(val));
			}
			should_rehash = false;
		}

		//Emplace without rehash and duplicate check
		template<class ...Args>
		iterator emplace_no_check(u64 bucket, Args&&...args) {
			if (sparse[bucket] == invalid_index) {
				sparse[bucket] = packed.size();			//create new bucket
				packed.emplace_back(std::forward<Args>(args)...);
				nodes.emplace_back();
			}
			else {
				//find the tail of the bucket
				u64 index = sparse[bucket];
				while (nodes[index].next != invalid_index) {
					index = nodes[index].next;
				}
				nodes[index].next = packed.size();
				packed.emplace_back(std::forward<Args>(args)...);
				nodes.emplace_back(index, invalid_index);
			}
			return packed.begin() + packed.size() - 1;		//return last iterator
		}

		template<class _Key = Key>
		iterator find(const _Key& key, u64 bucket) {
			//iterate the bucket
			u64 index = sparse[bucket];
			while (index != invalid_index) {
				if (myKeyeq(get_key(packed[index]), key)) {
					return packed.begin() + index;
				}
				index = nodes[index].next;
			}
			return packed.end();
		}

		template<class _Key = Key>
		const_iterator find(const _Key& key, u64 bucket)const {
			return const_cast<BaseMap*>(this)->find(key, bucket);
		}

		//Erase without checking index and bucket validity
		iterator erase_no_check(u64 index, u64 bucket) {
			//remove from linked list
			u64 prev = nodes[index].prev;
			u64 next = nodes[index].next;

			//rebind prev and next
			if (prev != invalid_index) {
				nodes[prev].next = next;
			}
			else {
				sparse[bucket] = next;
			}
			if (next != invalid_index) {
				nodes[next].prev = prev;
			}

			//if index is at back
			if (index == packed.size() - 1) {
				packed.pop_back();
				nodes.pop_back();
				return end();
			}

			//if index is not at back
			//rebind the back node
			if (nodes.back().prev != invalid_index) {
				nodes[nodes.back().prev].next = index;		//index is the new back position
			}
			else {
				u64 back_bucket = get_bucket(get_key(packed.back()));
				sparse[back_bucket] = index;
			}
			if (nodes.back().next != invalid_index) {
				nodes[nodes.back().next].prev = index;
			}
			std::swap(nodes[index], nodes.back());
			std::swap(packed[index], packed.back());
			nodes.pop_back();
			packed.pop_back();
			return packed.begin() + index;		//return the original back node
		}

		template<class _Key = Key>
		void throw_if_duplicated(u64 bucket, const _Key& key) {
			if (iterator it = find(key, bucket); it != packed.end()) {
				throw std::runtime_error("Duplicate key insertion in DenseMap");
			}
		}
	public:
		BaseMap() {
			sparse.assign(min_bucket_size, invalid_index);
			update_should_rehash();
		}
		BaseMap(const BaseMap&) = default;
		BaseMap(BaseMap&& other)noexcept :
			sparse(std::move(other.sparse)),
			packed(std::move(other.packed)),
			nodes(std::move(other.nodes)),
			myHash(std::move(other.myHash)),
			myKeyeq(std::move(other.myKeyeq)),
			should_rehash(other.should_rehash) {
			other.should_rehash = false;
		}

		BaseMap& operator=(BaseMap&& other)noexcept {
			if (this != &other) {
				sparse = std::move(other.sparse);
				packed = std::move(other.packed);
				nodes = std::move(other.nodes);
				myHash = std::move(other.myHash);
				myKeyeq = std::move(other.myKeyeq);
				should_rehash = other.should_rehash;
				other.should_rehash = false;
			}
			return *this;
		}

		//Guaranteed to be 2^x
		u64 bucket_count()const { return sparse.size(); }

		u64 size()const { return packed.size(); }

		bool empty()const { return packed.empty(); }

		iterator begin() { return packed.begin(); }
		iterator end() { return packed.end(); }
		const_iterator begin()const { return packed.begin(); }
		const_iterator end()const { return packed.end(); }

		void clear() {
			packed.clear();
			nodes.clear();
			sparse.assign(min_bucket_size, invalid_index);		//Made a huge fucking mistake here
			should_rehash = false;
		}

		template<class _Key = Key>
		iterator find(const _Key& key) {
			return find(key, get_bucket(key));
		}

		template<class _Key = Key>
		const_iterator find(const _Key& key)const {
			return find(key, get_bucket(key));
		}

		template<class _Key = Key>
		bool contains(const _Key& key)const {
			return find(key, get_bucket(key)) != end();
		}

		template<class _Key = Key>
		void erase(const _Key& key) {
			u64 bucket = get_bucket(key);
			iterator it = find(key, bucket);
			if (it == end())return;
			erase_no_check(static_cast<u64>(it - begin()), bucket);
		}

		iterator erase(iterator it) {
			return erase_no_check(static_cast<u64>(it - begin()), get_bucket(get_key(*it)));
		}

		/*template<class It>
		void insert(It _begin, It _end) {
			for (; _begin != _end; _begin++) {

			}
		}*/

		void merge(const BaseMap& other) {
			if (&other == this)return;
			for (auto& val : other) {
				emplace(val);
			}
		}

		Val& emplace(const Val& val) {
			rehash_if_should();
			auto& key = get_key(val);
			u64 bucket = get_bucket(key);
			throw_if_duplicated(bucket, key);
			auto& ret = *emplace_no_check(bucket, val);
			update_should_rehash();
			return ret;
		}
	};

	template<class _Key, class _Val, class _Hash, class _KeyEq, class _Alloc, bool _is_map>
	struct MapTraits {
		inline static constexpr bool is_map = _is_map;
		using Key = _Key;
		using Val = _Val;
		using Hash = _Hash;
		using KeyEq = _KeyEq;
		using Alloc = _Alloc;
	};


	template<class Key, class Type, class Hash = std::hash<Key>, class KeyEq = std::equal_to<Key>, class Alloc = std::allocator<void>>
	class DenseMap :public BaseMap<MapTraits<Key, std::pair<Key, Type>, Hash, KeyEq, Alloc, true>> {
	public:
		using Base = BaseMap<MapTraits<Key, std::pair<Key, Type>, Hash, KeyEq, Alloc, true>>;
		using Base::Base;
		using iterator = Base::iterator;

	private:
		using Pair = std::pair<Key, Type>;
	public:
		template<class _Key = Key, class ...Args>
		Pair& emplace_or_get(_Key&& key, Args&&...args) {
			Base::rehash_if_should();
			u64 bucket = Base::get_bucket(key);
			if (iterator it = Base::find(key, bucket); it != Base::packed.end()) {
				return *it;
			}
			Pair& ret = *Base::emplace_no_check(bucket,
												std::piecewise_construct,
												std::forward_as_tuple(std::forward<_Key>(key)),
												std::forward_as_tuple(std::forward<Args>(args)...));
			Base::update_should_rehash();
			return ret;
		}

		template<class _Key = Key, class ...Args>
		Pair& emplace(_Key&& key, Args&&...args) {
			Base::rehash_if_should();
			u64 bucket = Base::get_bucket(key);
			Base::throw_if_duplicated(bucket, key);
			auto& ret = *Base::emplace_no_check(bucket,
												std::piecewise_construct,
												std::forward_as_tuple(std::forward<_Key>(key)),
												std::forward_as_tuple(std::forward<Args>(args)...));
			Base::update_should_rehash();
			return ret;
		}

		/*Pair& emplace(::std::piecewise_construct_t, auto&& keyArg, auto&& valArg) {
			Base::rehash_if_should();
			auto key = Key(std::forward<decltype(keyArg)>(keyArg));
			u64 bucket = Base::get_bucket(key);
			Base::throw_if_duplicated(bucket, key);
			auto& ret = *Base::emplace_no_check(bucket,
												std::piecewise_construct,
												std::move(key),
												std::forward<decltype(valArg)>(valArg));
			Base::update_should_rehash();
			return ret;
		}*/

		template<class _Key = Key>
		Type& operator[](_Key&& key) {
			return emplace_or_get(std::forward<_Key>(key)).second;
		}
	};


	template<class Type, class Hash = std::hash<Type>, class KeyEq = std::equal_to<Type>, class Alloc = std::allocator<void>>
	class DenseSet :public BaseMap<MapTraits<Type, Type, Hash, KeyEq, Alloc, false>> {
	public:
		using Base = BaseMap<MapTraits<Type, Type, Hash, KeyEq, Alloc, false>>;
		using Base::Base;
		using iterator = Base::iterator;
	private:
		template<class _Type>
		iterator insert_no_check(_Type&& key) {
			Base::rehash_if_should();
			u64 bucket = Base::get_bucket(key);
			auto ret = Base::emplace_no_check(bucket, std::forward<_Type>(key));
			Base::update_should_rehash();
			return ret;
		}

	public:
		/*template<class _Type>
		iterator insert(_Type&& key) {
			if (Base::contains(key)) {
				throw std::runtime_error("Duplicate key insertion in DenseSet");
			}
			return insert_no_check(std::forward<_Type>(key));
		}*/

		//does not insert when it contains the key
		template<class _Type>
		iterator insert(_Type&& key) {
			if (auto it = Base::find(key); it != Base::end()) {
				return it;
			}
			return insert_no_check(std::forward<_Type>(key));
		}
	};
}
#endif