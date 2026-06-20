#pragma once
#include "src/utils/Integers.h"

#include <ranges>

namespace flx {
	template <class Derived, class T>
	struct Indexable {
		template <std::integral i = u64>
		auto indices() const {
			const auto& self = static_cast<const Derived&>(*this);
			return std::views::iota(i{0}, i(self.size()));
		}

		bool valid(int index) const {
			const auto& self = static_cast<const Derived&>(*this);
			return index >= 0 && index < self.size();
		}
	};

	template <class Derived, class T>
	struct Fillable {
		void fill(T elem) {
			auto& self = static_cast<Derived&>(*this);
			for (u64 i = 0; i < self.size(); ++i) {
				self.operator[](i) = elem;
			}
		}
	};

	template <class Derived, class T, template <class, class> class... Features>
	struct ContainerFeature : Features<Derived, T>... {};
} // namespace flx