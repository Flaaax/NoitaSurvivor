#pragma once
#ifndef MYECS_ENTITY_H
#define MYECS_ENTITY_H
#include"component.h"
#include"dense_map.h"
#include<unordered_set>
#include<iostream>

/*
	Triggering exceptions will cause ub in some cases!
*/

namespace myecs {

	namespace latest {
		//Single thread only
		//Doesn't require component move/copy constructible/movable
		//Safe to use multiple instances per program
		template<class Alloc = std::allocator<void>>
		class Registry {
		private:
			using component = u64;
			using entity_view = pool::entity_view;
			using PoolProxy = ComponentPoolProxy<Alloc>;
			using VoidAlloc = rebind_alloc<Alloc, void>;
			DenseMap<component, PoolProxy, std::identity, std::equal_to<component>, VoidAlloc> pools;
			IdGen<entity, VoidAlloc> ids;
			Vector<DenseSet<component, std::identity, std::equal_to<component>, VoidAlloc>> entity_components;

			template<class T>
			ComponentPool<T>& get_pool() {
				component id = types::type_id<T>();
				PoolProxy& proxy = pools[id];
				if (proxy.data.empty()) {
					proxy.emplace<T>();
				}
				return proxy.data.get<ComponentPool<T>>();
			}

			template<class T>
			ComponentPool<T>* try_get_pool() {
				component id = types::type_id<T>();
				if (auto it = pools.find(id); it != pools.end()) {
					PoolProxy& proxy = it->second;
					if (proxy.data.empty()) return {};
					return &proxy.data.get<ComponentPool<T>>();
				}
				return {};
			}

			template<class T>
			const ComponentPool<T>* try_get_pool()const {
				return const_cast<Registry*>(this)->try_get_pool<T>();
			}

			Registry(const Registry&) = delete;

			template<class T, class ...Types>
			struct poly_view_iterator {
				using it_t = SparseSet<entity>::const_iterator;
				it_t cur;
				it_t end;
				Registry& reg;

				bool shouldKeep() { return (reg.template has<Types>(*cur) && ...); }
				poly_view_iterator(it_t cur, it_t end, Registry& reg) :cur(cur), end(end), reg(reg) { move(); }
				void move() { while ((!stop()) && (!shouldKeep())) { cur++; } }
				bool stop()const { return cur == end; }
				void next() { cur++; move(); }
				auto get() { return std::tuple<entity, T&, Types&...>(*cur, reg.get<T>(*cur), reg.get<Types>(*cur)...); }
			};

		public:
			explicit Registry() = default;
			Registry(Registry&& other) noexcept :
				pools(std::move(other.pools)),
				ids(std::move(other.ids)),
				entity_components(std::move(other.entity_components)) {
			}

			//The reference would not expire unless you remove it.
			template<class T, class ...Args>
			T& emplace(entity e, Args&&... args) {
				if constexpr (myecs_debug_level) {
					if (!ids.active(e)) {
						throw std::runtime_error("invalid entity");
					}
				}
				u64 id = e.get_id();
				if (entity_components.size() <= id) {
					entity_components.resize(id + 1);
				}
				entity_components[id].insert(types::type_id<T>());
				ComponentPool<T>& pool = get_pool<T>();
				return pool.create(e, std::forward<Args>(args)...);
			}

			template<class T, class ...Args>
			T& get_or_emplace(entity e, Args&&... args) {
				ComponentPool<T>& pool = get_pool<T>();
				if (pool.has(e)) return pool.get(e);
				return emplace<T>(e, std::forward<Args>(args)...);
			}

			template<class T, class...Args>
			T& emplace_or_replace(entity e, Args&&...args) {
				ComponentPool<T>& pool = get_pool<T>();
				if (pool.has(e))return pool.replace(e, std::forward<Args>(args)...);
				return emplace<T>(e, std::forward<Args>(args)...);
			}

			template<class T>
			MYECS_NODISCARD bool has(entity e)const {
				if (const ComponentPool<T>* pool = try_get_pool<T>()) {
					return pool->has(e);
				}
				return false;
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD bool has(entity e)const {
				return (has<Types>(e) && ...);
			}

			//warning: when you emplace new component, the reference may expire!
			template<class T>
			MYECS_NODISCARD T& get(entity e) {
				if constexpr (myecs_debug_level) {
					if (!valid(e)) {
						throw std::runtime_error("invalid entity");
					}
				}
				ComponentPool<T>& pool = get_pool<T>();
				return pool.get(e);
			}

			template<class T>
			MYECS_NODISCARD T* try_get(entity e) {
				if (!has<T>(e)) {
					return nullptr;
				}
				return &get<T>(e);
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD decltype(auto) try_get(entity e) {
				return std::make_tuple(try_get<Types>(e)...);
			}

			template<class T>
			void destroy(entity e) {
				if (!ids.active(e)) {
					throw std::runtime_error("Entity is invalid");
				}
				if (auto pool = try_get_pool<T>()) {
					u64 id = e.get_id();
					if (entity_components.size() <= id) {
						throw std::runtime_error("Entity does not have component to destroy");
					}
					entity_components[id].erase(types::type_id<T>());
					pool->destroy(e);
				}
				else throw std::runtime_error("Entity does not have component to destroy");
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			void destroy(entity e) {
				(destroy<Types>(e), ...);
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD std::tuple<Types&...> get(entity e) {
				return std::forward_as_tuple(get<Types>(e)...);
			}

			MYECS_NODISCARD entity create() {
				return ids.get();
			}

			void destroy(entity e) {
				if (!ids.active(e)) {
					return;
				}
				ids.ret(e);
				u64 id = static_cast<u64>(e.id);
				if (entity_components.size() <= id) return;
				for (auto cid : entity_components[id]) {
					pools[cid].proxy_destroy(e);
				}
				entity_components[id].clear();
			}

			MYECS_NODISCARD bool valid(entity e)const {
				return ids.active(e);
			}

			//template<class ...Args>
			//	requires (std::is_same_v<Args, entity> && ...) && (sizeof...(Args) >= 2)
			//MYECS_NODISCARD bool all_valid(Args... es)const {
			//	return (valid(es) && ...);
			//}

			template<class T>
			MYECS_NODISCARD decltype(auto) view() {
				return get_pool<T>().view();
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD decltype(auto) view() {
				using T1 = std::tuple_element_t<0, std::tuple<Types...>>;
				auto& es = get_pool<T1>().entity_view();
				return Iterable(poly_view_iterator<Types...>{ es.begin(), es.end(), * this });
			}

			//clear all the items inside the register
			void reset() {
				ids.clear();
				entity_components.clear();
				for (auto& [id, pool] : pools) {
					pool.proxy_clear();
				}
			}

			u64 entity_count()const {
				return ids.count();
			}

			u64 max_entity_count()const {
				return ids.max_count();
			}

			u64 component_count()const {
				u64 ret = {};
				for (auto& [id, pool] : pools) {
					ret += pool.proxy_count();
				}
				return ret;
			}

			u64 max_component_count()const {
				u64 ret = {};
				for (auto& [id, pool] : pools) {
					ret += pool.proxy_max_count();
				}
				return ret;
			}
		};
	}

	/*
	namespace old {
		//Single thread only
		//Doesn't require component move/copy constructible/movable
		//One instance per program suggested
		class Registry {
		private:
			using component = pool::component;
			using component_type = id_type;
			using entity_view = pool::entity_view;
			using PoolProxy = ComponentPoolProxy;

			class _ComponentRegistry {
			private:
				inline static id_type component_id_reg = 0;
			public:
				template<class T>
				static id_type getComponentId() {
					static id_type _id = component_id_reg++;
					return _id;
				}

			};

			std::vector<PoolProxy> pools;
			IdGen<entity> ids;
			std::vector<SparseSet<id_type>> entity_components;

			template<class T>
			ComponentPool<T>& get_pool() {
				id_type component_id = _ComponentRegistry::getComponentId<T>();
				if (pools.size() <= component_id) {
					pools.resize(component_id + 1);
				}
				PoolProxy& proxy = pools[component_id];
				if (!proxy.has_value()) {
					proxy.emplace<T>();
				}
				return proxy.get<T>();
			}

			template<class T>
			ComponentPool<T>* try_get_pool() {
				id_type component_id = _ComponentRegistry::getComponentId<T>();
				if (pools.size() <= component_id) {
					return nullptr;
				}
				PoolProxy& proxy = pools[component_id];
				if (!proxy.has_value()) {
					return nullptr;
				}
				return &proxy.get<T>();
			}

			template<class T>
			const ComponentPool<T>* try_get_pool()const {
				return const_cast<Registry*>(this)->try_get_pool<T>();
			}

			Registry(const Registry&) = delete;

			template<class T, class ...Types>
			struct poly_view_iterator {
				using it_t = SparseSet<entity>::const_iterator;
				it_t cur;
				it_t end;
				Registry& reg;

				bool shouldKeep() { return (reg.has<Types>(*cur) && ...); }
				poly_view_iterator(it_t cur, it_t end, Registry& reg) :cur(cur), end(end), reg(reg) { move(); }
				void move() { while ((!stop()) && (!shouldKeep())) { cur++; } }
				bool stop()const { return cur == end; }
				void next() { cur++; move(); }
				auto get() { return std::tuple<entity, T&, Types&...>(*cur, reg.get<T>(*cur), reg.get<Types>(*cur)...); }
			};

		public:
			explicit Registry() = default;
			Registry(Registry&& other) noexcept :
				pools(std::move(other.pools)),
				ids(std::move(other.ids)),
				entity_components(std::move(other.entity_components)) {
			}

			//The reference would not expire until you remove it.
			template<class T, class ...Args>
			T& emplace(entity e, Args&&... args) {
				if constexpr (myecs_debug_level) {
					if (!ids.active(e)) {
						throw std::runtime_error("invalid entity");
					}
				}
				u64 id = e.get_id();
				if (entity_components.size() <= id) {
					entity_components.resize(id + 1);
				}
				entity_components[id].insert(_ComponentRegistry::getComponentId<T>());
				ComponentPool<T>& pool = get_pool<T>();
				return pool.create(e, std::forward<Args>(args)...);
			}

			template<class T, class ...Args>
			T& get_or_emplace(entity e, Args&&... args) {
				ComponentPool<T>& pool = get_pool<T>();
				if (pool.has(e)) {
					return pool.get(e);
				}
				else {
					return emplace<T>(e, std::forward<Args>(args)...);
				}
			}

			template<class ...Types>
			decltype(auto) emplace_all(entity e, const Types&... types) {
				return std::forward_as_tuple(emplace<Types>(e, types)...);
			}

			template<class T>
			MYECS_NODISCARD bool has(entity e)const {
				if (const ComponentPool<T>* pool = try_get_pool<T>()) {
					return pool->has(e);
				}
				return false;
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD bool has(entity e)const {
				return (has<Types>(e) && ...);
			}

			//warning: when you emplace new component, the reference may expire!
			template<class T>
			MYECS_NODISCARD T& get(entity e) {
				if constexpr (myecs_debug_level) {
					if (!valid(e)) {
						throw std::runtime_error("invalid entity");
					}
				}
				ComponentPool<T>& pool = get_pool<T>();
				return pool.get(e);
			}

			template<class T>
			MYECS_NODISCARD T* try_get(entity e) {
				if (!has<T>(e)) {
					return nullptr;
				}
				return &get<T>(e);
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD decltype(auto) try_get(entity e) {
				return std::make_tuple(try_get<Types>(e)...);
			}

			template<class T>
			void destroy(entity e) {
				if (auto pool = try_get_pool<T>()) {
					pool->destroy(e);
					u64 id = e.get_id();
					if (entity_components.size() <= id) {
						return;
					}
					entity_components[id].erase(_ComponentRegistry::getComponentId<T>());
				}
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			void destroy(entity e) {
				(destroy<Types>(e), ...);
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD std::tuple<Types&...> get(entity e) {
				return std::forward_as_tuple(get<Types>(e)...);
			}

			MYECS_NODISCARD entity create() {
				return ids.get();
			}

			void destroy(entity e) {
				if (!ids.active(e)) {
					return;
				}
				ids.ret(e);
				u64 id = static_cast<u64>(e.id);
				if (entity_components.size() <= id) {
					return;
				}
				for (auto cid : entity_components[id]) {
					pools[cid].proxy_destroy(e);
				}
				entity_components[id].clear();
			}

			MYECS_NODISCARD bool valid(entity e)const {
				return ids.active(e);
			}

			template<class T>
			MYECS_NODISCARD decltype(auto) view() {
				return get_pool<T>().view();
			}

			template<class ...Types>
				requires (sizeof...(Types) >= 2)
			MYECS_NODISCARD decltype(auto) view() {
				using T1 = std::tuple_element_t<0, std::tuple<Types...>>;
				auto& es = get_pool<T1>().entity_view();
				return Iterable(poly_view_iterator<Types...>{ es.begin(), es.end(), * this });
			}

			//clear all the items inside the register
			void reset() {
				ids.clear();
				entity_components.clear();
				for (auto& pool : pools) {
					pool.proxy_clear();
				}
			}

			u64 entity_count()const {
				return ids.count();
			}

			u64 max_entity_count()const {
				return ids.max_count();
			}

			u64 component_count()const {
				u64 ret = {};
				for (const auto& pool : pools) {
					ret += pool.proxy_count();
				}
				return ret;
			}

			u64 max_component_count()const {
				u64 ret = {};
				for (const auto& pool : pools) {
					ret += pool.proxy_max_count();
				}
				return ret;
			}
		};
	}
	*/

	using Registry = latest::Registry<>;

}//namespace myecs


#endif