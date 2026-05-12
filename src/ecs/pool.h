#pragma once
#include<unordered_map>
#include<stdexcept>
#include<optional>
#include<deque>
#include"types.h"


namespace myecs {

	namespace pool {
	#pragma warning(push)
	#pragma warning(disable:26495)
		//info currently unused
		template<class Base, u64 Size>
		class ClassData {
		private:
			unsigned char data[Size];
			void (*move_construct)(void* self_data, void* other_data) = nullptr;

		public:
			ClassData() {}
			ClassData(ClassData&& other)noexcept :move_construct(other.move_construct) {
				if (move_construct) {
					move_construct(data, other.data);
				}
			}
			~ClassData() {
				destroy();
			}

			Base* get() {
				return reinterpret_cast<Base*>(data);
			}

			const Base* get()const {
				return reinterpret_cast<const Base*>(data);
			}

			template<class T>
				requires std::derived_from<T, Base>
			T* get() {
				return reinterpret_cast<T*>(data);
			}

			template<class T>
				requires std::derived_from<T, Base>
			const T* get()const {
				return reinterpret_cast<const T*>(data);
			}

			void destroy() {
				if (has_value()) {
					move_construct = nullptr;
					get()->~Base();
				}
			}

			template<class T, class ...Args>
				requires std::derived_from<T, Base>
			void emplace(Args&&... args) {
				destroy();
				new (data) T(std::forward<Args>(args)...);
				move_construct = [](void* self_data, void* other_data) {
					new (self_data) T(std::move(*reinterpret_cast<T*>(other_data)));
				};
			}

			bool has_value()const {
				return (bool)(move_construct);
			}
		};
	#pragma warning(pop)

		template<class I, class Alloc = std::allocator<void>>
		class BasePool {
		public:
			IdGen<I, Alloc> ids;

			u64 count()const {
				return ids.count();
			}
			u64 max_count()const {
				return ids.max_count();
			}
			bool valid(I id)const {
				return ids.active(id);
			}
		};

		//All functions don't check vadility
		template<class T, class Alloc = std::allocator<void>>
		class Pool :public BasePool<u64, Alloc> {
		private:
			using Base = BasePool<u64, Alloc>;
			using Base::Base;
			std::deque<std::optional<T>, rebind_alloc<Alloc, std::optional<T>>> storage;		

		public:

			template<class ...Args>
			std::pair<u64, T&> create(Args&&... args) {
				if (this->ids.full()) {
					storage.emplace_back();
				}
				u64 id = this->ids.get();
				auto& ret = storage[id].emplace(std::forward<Args>(args)...);
				return { id, ret };
			}

			//id must be valid
			template<class ...Args>
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
	}//namespace pool

}//namespace myecs