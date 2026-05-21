#pragma once
#include<unordered_map>
#include<stdexcept>
#include<optional>
#include<deque>
#include"types.h"


namespace myecs::pool {
	template <class I, class Alloc = std::allocator<void>>
	class BasePool {
	public:
		IdGen<I, Alloc> ids;

		u64 count() const {
			return ids.count();
		}

		u64 max_count() const {
			return ids.max_count();
		}

		bool valid(I id) const {
			return ids.active(id);
		}
	};

	// All functions don't check vadility
	template <class T, class Alloc = std::allocator<void>>
	class Pool : public BasePool<u64, Alloc> {
	private:
		using Base = BasePool<u64, Alloc>;
		using Base::Base;
		std::deque<std::optional<T>, rebind_alloc<Alloc, std::optional<T>>> storage;

	public:
		template <class... Args>
		std::pair<u64, T&> create(Args&&... args) {
			if (this->ids.full()) {
				storage.emplace_back();
			}
			u64 id = this->ids.get();
			try {
				auto& ret = storage[id].emplace(std::forward<Args>(args)...);
				return {id, ret};
			} catch (...) {
				this->ids.ret(id);
				throw;
			}
		}

		// id must be valid
		template <class... Args>
		T& replace(u64 id, Args&&... args) {
			return storage[id].emplace(std::forward<Args>(args)...);
		}

		T& get(u64 id) {
			return storage[id].value();
		}

		T* try_get(u64 id) {
			return this->valid(id) ? &get(id) : nullptr;
		}

		void destroy(u64 id) {
			storage[id].reset();
			this->ids.ret(id);
		}

		void clear() {
			storage.clear();
			this->ids.clear();
		}
	};
} // namespace myecs::pool