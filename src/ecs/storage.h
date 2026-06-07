#pragma once
#include "container.h"

#include "types.h"
#include <deque>
#include <optional>

namespace myecs::storage {
	template <class T, class Alloc_, u64 page_size>
		requires(is_power_of_two(page_size))
	struct ComponentStorage {
		using Alloc = rebind_alloc<Alloc_, T>;
		Alloc alloc;
		Vector<T*, Alloc_> pages{};

		ComponentStorage(ComponentStorage&&) = default;

		explicit ComponentStorage(const Alloc& alloc = {}) : alloc(alloc) {}

		~ComponentStorage() {
			for (T* page : pages) {
				if (page) {
					alloc.deallocate(page, page_size);
				}
			}
		}

		static auto unwrap(u64 index) {
			u64 page_index = index / page_size;
			u64 offset = index % page_size;
			return std::pair{page_index, offset};
		}

		T* force_get_page(u64 page_index) {
			T*& page = pages.force_get(page_index);
			if (!page) {
				page = alloc.allocate(page_size);
			}
			return page;
		}

		T* get_page(u64 page_index) {
			return pages[page_index];
		}

		// Does not check if component valid...
		T& get(u64 index) {
			auto [page_index, offset] = unwrap(index);
			return pages[page_index][offset];
		}

		template <class... Args>
		T& emplace(u64 index, Args&&... args) {
			auto [page_index, offset] = unwrap(index);
			T* page = this->force_get_page(page_index);
			return *std::construct_at<T>(page + offset, std::forward<Args>(args)...);
		}

		template <class... Args>
		T& replace(u64 index, Args&&... args) {
			auto [page_index, offset] = unwrap(index);
			T* page = this->force_get_page(page_index);
			std::destroy_at<T>(page + offset);
			return *std::construct_at<T>(page + offset, std::forward<Args>(args)...);
		}

		void destroy(u64 index) {
			auto [page_index, offset] = unwrap(index);
			T* page = this->get_page(page_index);
			std::destroy_at<T>(page + offset);
		}
	};
} // namespace myecs::storage