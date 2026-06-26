#pragma once
#include "src/utils/Integers.h"

#include <ranges>

namespace flx {
	namespace internal {
		template <template <class...> class Feature, class Derived, class... Args>
		Derived& self(Feature<Derived, Args...>* ptr) {
			return static_cast<Derived&>(*ptr);
		}

		template <template <class...> class Feature, class Derived, class... Args>
		const Derived& self(const Feature<Derived, Args...>* ptr) {
			return static_cast<const Derived&>(*ptr);
		}
	} // namespace internal

	template <class Derived, class T>
	struct Indexable {
		template <std::integral I = u64>
		auto indices() const {
			return std::views::iota(I{0}, I(internal::self(this).size()));
		}

		bool valid(int index) const {
			const auto& self = internal::self(this);
			return index >= 0 && index < self.size();
		}

		template <std::integral I = u64>
		auto enumerate() const {
			return indices<I>() | std::views::transform([this](I i) -> std::pair<I, const T&> { return {i, internal::self(this)[i]}; });
		}
	};

	template <class Derived, class T>
	struct Fillable {
		void fill(T elem) {
			auto& self = internal::self(this);
			for (u64 i = 0; i < self.size(); ++i) {
				self[i] = elem;
			}
		}
	};

	template <class Derived, class T, template <class, class> class... Features>
	struct ContainerFeature : Features<Derived, T>... {};
} // namespace flx