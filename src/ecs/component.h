#pragma once
#ifndef MYECS_COMPONENT_H
#define MYECS_COMPONENT_H
#include"container.h"
#include"pool.h"
#include"variant.h"


namespace myecs {
	namespace pool {
		using component = u64;
		using entity_view = const SparseSet<entity>&;
	}

	template<class T, class Alloc = std::allocator<void>>
	class ComponentPool {
	private:
		using component = pool::component;
		using entity_view_t = pool::entity_view;
		SparseSet<entity, Alloc> entities;
		Vector<component, Alloc> entity_to_component;
		pool::Pool<T, Alloc> pool;

	public:
		ComponentPool() {}
		ComponentPool(ComponentPool&&) = default;
		ComponentPool& operator=(ComponentPool&&) = default;

		bool has(entity e)const {
			return entities.has(e);
		}

		entity_view_t entity_view()const {
			return entities;
		}

		auto view() {
			struct It {
				SparseSet<entity>::const_iterator it;
				SparseSet<entity>::const_iterator end;
				ComponentPool& pool;

				bool stop()const { return it == end; }
				std::pair<entity, T&> get() { return { *it, pool.get(*it) }; }
				void next() { ++it; }
			};
			It it{ entities.begin(), entities.end(), *this };
			return Iterable(it);
		}

		template<class ...Args>
		T& create(entity e, Args&&... args) {
			if (has(e)) {
				throw std::runtime_error("entity already has component");
			}
			auto [id, ret] = pool.create(std::forward<Args>(args)...);
			entities.insert(e);
			entity_to_component.force_get(e.get_id()) = id;
			return ret;
		}

		//replace the component in place
		template<class ...Args>
		T& replace(entity e, Args&&...args) {
			if (!has(e)) {
				throw std::runtime_error("entity must have the component");
			}
			component c = entity_to_component[e.get_id()];
			return pool.replace(c, std::forward<Args>(args)...);
		}

		T& get(entity e) {
			MYECS_ASSERT(has(e), "invalid entity");
			component c = entity_to_component[e.get_id()];
			return pool.get(c);
		}

		void clear() {
			pool.clear();
			entities.clear();
			entity_to_component.clear();
		}

		void destroy(entity e) {
			if (!has(e)) return;
			entities.erase(e);
			auto c = entity_to_component[e.get_id()];
			pool.destroy(c);
		}

		u64 count()const {
			return pool.count();
		}

		u64 max_count()const {
			return pool.max_count();
		}
	};

	template<class Alloc>
	struct ComponentPoolProxy {
		struct vtable {
			void (*m_destroy)(void* self, entity e) = {};
			void (*m_clear)(void* self) = {};
			u64 (*m_count)(const void* self) = {};
			u64 (*m_max_count)(const void* self) = {};
		};
		vtable table;
		Variant<sizeof(ComponentPool<int, Alloc>)> data;

		ComponentPoolProxy() {}
		ComponentPoolProxy(ComponentPoolProxy&& other)noexcept :
			table(move_and_reset(other.table)), data(move_and_reset(other.data)) {
		}
		ComponentPoolProxy& operator=(ComponentPoolProxy&& other)noexcept {
			table = move_and_reset(other.table);
			data = move_and_reset(other.data);
			return *this;
		}

		template<class T>
		void emplace() {
			using pool_t = ComponentPool<T, Alloc>;
			data.clear();
			data.emplace<pool_t>();
			table = vtable{
				.m_destroy = [](void* self, entity e) {
					static_cast<pool_t*>(self)->destroy(e);
				},
				.m_clear = [](void* self) {
					static_cast<pool_t*>(self)->clear();
				},
				.m_count = [](const void* self) {
					return static_cast<const pool_t*>(self)->count();
				},
				.m_max_count = [](const void* self) {
					return static_cast<const pool_t*>(self)->max_count();
				}
			};
		}

		void proxy_destroy(entity e) {
			table.m_destroy(data.pointer(), e);
		}
		void proxy_clear() {
			table.m_clear(data.pointer());
		}
		u64 proxy_count()const {
			return table.m_count(data.pointer());
		}
		u64 proxy_max_count()const {
			return table.m_max_count(data.pointer());
		}
	};

}//namespace myecs


#endif