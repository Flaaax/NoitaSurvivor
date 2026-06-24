#pragma once
#ifndef MYECS_COMPONENT_H
#define MYECS_COMPONENT_H
#include "container.h"
#include "storage.h"
#include "variant.h"

#include <ranges>

namespace myecs {
	template <class T, Policy policy>
	struct ComponentPolicy {
		static constexpr bool in_place_delete = [] {
			if (!std::is_nothrow_move_constructible_v<T>) {
				return true;
			}
			if constexpr (requires { T::in_place_delete; }) {
				return static_cast<bool>(T::in_place_delete);
			} else {
				return policy.in_place_delete;
			}
		}();

		// page_size must be power of 2
		static constexpr u64 page_size = [] {
			if constexpr (requires { T::page_size; }) {
				return static_cast<u64>(T::page_size);
			} else {
				return policy.page_size;
			}
		}();
	};

	template <class T, class Alloc, Policy policy>
	class ComponentPool {
	private:
		using Policy = ComponentPolicy<T, policy>;
		static constexpr bool in_place_delete = Policy::in_place_delete;
		static constexpr u64 page_size = Policy::page_size;
		// using Indicator = std::conditional_t<in_place_delete, u32, internal::Dummy>;

		Vector<entity, Alloc> packed{};
		Vector<u64, Alloc> sparse{};
		storage::ComponentStorage<T, Alloc, page_size> storage{};
		u32 free_head = u32_max; // for in_place_delete policy
		u64 count_{};

	public:
		explicit ComponentPool(const Alloc& alloc = {})
			: packed(alloc),
			  sparse(alloc),
			  storage(alloc) {
		}

		ComponentPool(ComponentPool&&) = default;
		ComponentPool& operator=(ComponentPool&&) = delete;

		~ComponentPool() {
			clear();
		}

		bool has(entity e) const {
			if (e.id >= sparse.size()) {
				return false;
			}
			const u64 packed_index = sparse[e.id];
			return packed_index < packed.size() && packed[packed_index] == e;
		}

		auto entity_view() const {
			if constexpr (in_place_delete) {
				return std::views::all(packed) |
					   std::views::filter([this](entity e) { return e.id != u32_max; });
			} else {
				return std::views::all(packed);
			}
		}

		auto view() {
			if constexpr (in_place_delete) {
				return std::views::all(packed) |
					   std::views::filter([this](entity e) { return e.id != u32_max; }) |
					   std::views::transform([this](entity e) -> std::pair<entity, T&> {
						   return {e, get(e)};
					   });
			} else {
				return std::views::all(packed) |
					   std::views::transform([this](entity e) -> std::pair<entity, T&> {
						   return {e, get(e)};
					   });
			}
		}

		template <class... Args>
		T& create(entity e, Args&&... args) {
			if (has(e)) {
				throw std::runtime_error("entity already has component");
			}

			++count_;

			const u64 index = e.id;
			u64 packed_index;

			if constexpr (in_place_delete) {
				if (free_head == u32_max) {
					// packed is full, append to the end
					packed_index = packed.size();
					sparse.force_get(index, u64_max) = packed_index;
					packed.emplace_back(e);
				} else {
					// Find the next valid spot through linked list
					packed_index = free_head;
					sparse.force_get(index, u64_max) = packed_index;
					free_head = packed[free_head].version; // Points to next invalid entity
					packed[packed_index] = e;
				}
			} else {
				// packed is always full
				packed_index = packed.size();
				sparse.force_get(index, u64_max) = packed_index;
				packed.emplace_back(e);
			}

			return storage.emplace(packed_index, std::forward<Args>(args)...);
		}

		// replace the component in place
		template <class... Args>
		T& replace(entity e, Args&&... args) {
			if (!has(e)) {
				throw std::runtime_error("entity must have the component");
			}
			return storage.replace(sparse[e.id], std::forward<Args>(args)...);
		}

		T& get(entity e) {
			MYECS_ASSERT(has(e), "invalid entity");
			return storage.get(sparse[e.id]);
		}

		void clear() {
			if constexpr (in_place_delete) {
				for (u64 i = 0; i < packed.size(); ++i) {
					if (packed[i].id != u32_max) {
						storage.destroy(i);
					}
				}
				free_head = u32_max;
			} else {
				for (u64 i = 0; i < packed.size(); ++i) {
					storage.destroy(i);
				}
			}

			packed.clear();
			sparse.clear();
			count_ = 0;
		}

		void destroy(entity e) {
			if (!has(e)) {
				return;
			}

			--count_;

			const u64 sparse_index = e.id;
			const u64 packed_index = sparse[sparse_index];
			sparse[sparse_index] = u64_max;

			storage.destroy(packed_index);

			if constexpr (in_place_delete) {
				packed[packed_index] = {u32_max, free_head};
				free_head = static_cast<u32>(packed_index);
			} else {
				const u64 last_index = packed.size() - 1;
				if (last_index == packed_index) {
					packed.pop_back();
				} else {
					packed[packed_index] = packed[last_index];
					sparse[packed[last_index].id] = packed_index;
					packed.pop_back();
					T& last_storage = storage.get(last_index);
					storage.emplace(packed_index, std::move(last_storage));
					storage.destroy(last_index);
				}
			}
		}

		u64 count() const {
			return count_;
		}
	};

	template <class Alloc, Policy policy>
	struct ComponentPoolProxy {
		struct vtable {
			void (*m_destroy)(void* self, entity e) = {};
			void (*m_clear)(void* self) = {};
			u64 (*m_count)(const void* self) = {};
			// u64 (*m_max_count)(const void* self) = {};
		};

		vtable table;
		Variant<sizeof(ComponentPool<int, Alloc, policy>)> data;

		ComponentPoolProxy() {}

		ComponentPoolProxy(ComponentPoolProxy&& other) noexcept
			: table(std::move(other.table)), data(std::move(other.data)) {
			other.table = {};
		}

		ComponentPoolProxy& operator=(ComponentPoolProxy&& other) noexcept {
			table = std::move(other.table);
			data = std::move(other.data);
			other.data = {};
			return *this;
		}

		template <class T>
		void emplace() {
			using pool_t = ComponentPool<T, Alloc, policy>;
			data.clear();
			data.template emplace<pool_t>();
			table = vtable{
				.m_destroy = [](void* self, entity e) { static_cast<pool_t*>(self)->destroy(e); },
				.m_clear = [](void* self) { static_cast<pool_t*>(self)->clear(); },
				.m_count = [](const void* self) { return static_cast<const pool_t*>(self)->count(); },
			};
			//.m_max_count = [](const void* self) { return static_cast<const pool_t*>(self)->max_count(); }};
		}

		void proxy_destroy(entity e) {
			table.m_destroy(data.pointer(), e);
		}

		void proxy_clear() {
			table.m_clear(data.pointer());
		}

		u64 proxy_count() const {
			return table.m_count(data.pointer());
		}

		//
		// u64 proxy_max_count() const {
		// 	return table.m_max_count(data.pointer());
		// }
	};

} // namespace myecs

#endif