#pragma once
#ifndef MYECS_ENTITY_H
#define MYECS_ENTITY_H
#include "component.h"
#include "dense_map.h"

/*
	Triggering exceptions will cause ub in some cases!
*/

namespace myecs {
	namespace latest {
		// Thread-unsafe
		// Move/Copy requirement for compmonents: None
		template <Policy policy = {}, class Alloc = std::allocator<void>>
		class Registry {
		private:
			using component_id = u64;
			using VoidAlloc = rebind_alloc<Alloc, void>;
			using PoolProxy = ComponentPoolProxy<VoidAlloc, policy>;
			template <class T>
			using Pool = ComponentPool<T, VoidAlloc, policy>;

			DenseMap<component_id, PoolProxy, std::identity, std::equal_to<>, VoidAlloc> pools;
			IdGen<VoidAlloc> ids;
			Vector<DenseSet<component_id, std::identity, std::equal_to<>, VoidAlloc>> entity_components;

			template <class T>
			Pool<T>& get_pool() {
				component_id id = types::type_id<T>();
				PoolProxy& proxy = pools[id];
				if (proxy.data.empty()) {
					proxy.template emplace<T>();
				}
				return proxy.data.template get<Pool<T>>();
			}

			template <class T>
			Pool<T>* try_get_pool() {
				component_id id = types::type_id<T>();
				if (auto it = pools.find(id); it != pools.end()) {
					PoolProxy& proxy = it->second;
					if (proxy.data.empty())
						return {};
					return &proxy.data.template get<Pool<T>>();
				}
				return {};
			}

			template <class T>
			const Pool<T>* try_get_pool() const {
				return const_cast<Registry*>(this)->try_get_pool<T>();
			}


		public:
			explicit Registry() = default;
			Registry(const Registry&) = delete;

			Registry(Registry&& other) noexcept
				: pools(std::move(other.pools)), ids(std::move(other.ids)), entity_components(std::move(other.entity_components)) {
			}

			// The reference would not expire unless you remove it.
			template <class T, class... Args>
			T& emplace(entity e, Args&&... args) {
				if constexpr (myecs_debug_level) {
					throw_if(!ids.active(e), "Invalid entity: {}", e.string());
				}
				u64 id = e.id_u64();
				if (entity_components.size() <= id) {
					entity_components.resize(id + 1);
				}
				entity_components[id].insert(types::type_id<T>());
				Pool<T>& pool = get_pool<T>();
				return pool.create(e, std::forward<Args>(args)...);
			}

			template <class T, class... Args>
			T& get_or_emplace(entity e, Args&&... args) {
				// throw_if(!valid(e), "Entity {} is invalid", e.string());
				if (Pool<T>& pool = get_pool<T>(); pool.has(e))
					return pool.get(e);
				return emplace<T>(e, std::forward<Args>(args)...);
			}

			template <class T, class... Args>
			T& emplace_or_replace(entity e, Args&&... args) {
				// throw_if(!valid(e), "Entity {} is invalid", e.string());
				if (Pool<T>& pool = get_pool<T>(); pool.has(e))
					return pool.replace(e, std::forward<Args>(args)...);
				return emplace<T>(e, std::forward<Args>(args)...);
			}

			template <class T>
			MYECS_NODISCARD bool has(entity e) const {
				if (!valid(e))
					return false;
				if (const Pool<T>* pool = try_get_pool<T>()) {
					return pool->has(e);
				}
				return false;
			}

			template <class... Types>
				requires(sizeof...(Types) >= 2)
			MYECS_NODISCARD bool has(entity e) const {
				return (has<Types>(e) && ...);
			}

			template <class T>
			MYECS_NODISCARD T& get(entity e) {
				throw_if(!valid(e), "Entity {} is invalid", e.string());
				Pool<T>& pool = get_pool<T>();
				return pool.get(e);
			}

			template <class T>
			MYECS_NODISCARD std::tuple<T&, T&> get(entity a, entity b) {
				return std::tie(get<T>(a), get<T>(b));
			}

			template <class T>
			MYECS_NODISCARD T* try_get(entity e) {
				if (!valid(e))
					return {};
				Pool<T>* pool = try_get_pool<T>();
				if (!pool || !pool->has(e))
					return {};
				return &pool->get(e);
			}

			template <class T>
			MYECS_NODISCARD const T* try_get(entity e) const {
				if (!valid(e))
					return {};
				const Pool<T>* pool = try_get_pool<T>();
				if (!pool || !pool->has(e))
					return {};
				return &pool->get(e);
			}

			template <class T>
			MYECS_NODISCARD std::array<T*, 2> try_get(entity e1, entity e2) {
				return {try_get<T>(e1), try_get<T>(e2)};
			}

			template <class... Types>
				requires(sizeof...(Types) >= 2)
			MYECS_NODISCARD decltype(auto) try_get(entity e) {
				return std::make_tuple(try_get<Types>(e)...);
			}

			template <class T>
			void destroy(entity e) {
				throw_if(!valid(e), "Trying to destroy Component {} from invalid entity {}", types::type_name<T>(), e.string());
				if (auto pool = try_get_pool<T>()) {
					u64 id = e.id_u64();
					throw_if(entity_components.size() <= id, "Entity {} does not have Component {}, but trying to destroy it.", e.string(),
							 types::type_name<T>());
					entity_components[id].erase(types::type_id<T>());
					pool->destroy(e);
				} else
					throw_format("Entity {} does not have Component {}, but trying to destroy it.", e.string(), types::type_name<T>());
			}

			template <class... Types>
				requires(sizeof...(Types) >= 2)
			void destroy(entity e) {
				(destroy<Types>(e), ...);
			}

			template <class... Types>
				requires(sizeof...(Types) >= 2)
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
				u64 id = e.id;
				if (entity_components.size() <= id)
					return;
				for (auto cid : entity_components[id]) {
					pools[cid].proxy_destroy(e);
				}
				entity_components[id].clear();
			}

			MYECS_NODISCARD bool valid(entity e) const {
				return ids.active(e);
			}

			template <class T>
			MYECS_NODISCARD decltype(auto) view() {
				return get_pool<T>().view();
			}

			template <class T, class... Types>
				requires(sizeof...(Types) >= 1)
			MYECS_NODISCARD decltype(auto) view() {
				auto es = get_pool<T>().entity_view();

				return std::views::all(es) |
					   std::views::filter([this](entity e) { return (has<Types>(e) && ...); }) |
					   std::views::transform([this](entity e) {
						   return std::tuple<entity, T&, Types&...>{
							   e,
							   get<T>(e),
							   get<Types>(e)...};
					   });
			}

			// Clear everything inside the register
			void reset() {
				ids.clear();
				entity_components.clear();
				for (auto& [id, pool] : pools) {
					pool.proxy_clear();
				}
			}

			MYECS_NODISCARD u64 entity_count() const {
				return ids.count();
			}

			MYECS_NODISCARD u64 max_entity_count() const {
				return ids.max_count();
			}

			MYECS_NODISCARD u64 component_count() const {
				u64 ret = {};
				for (auto& [id, pool] : pools) {
					ret += pool.proxy_count();
				}
				return ret;
			}

			// u64 max_component_count() const {
			// 	u64 ret = {};
			// 	for (auto& [id, pool] : pools) {
			// 		ret += pool.proxy_max_count();
			// 	}
			// 	return ret;
			// }
		};
	} // namespace latest

	using Registry = latest::Registry<>;
} // namespace myecs

#endif
