#pragma once
#include <algorithm>
#include <ranges>

namespace flx {
	template <std::ranges::view V>
	class BasicView;

	namespace internal {
		template <std::ranges::range Range>
		[[nodiscard]]
		auto makeView(Range&& range) {
			return BasicView<std::views::all_t<Range>>(
				std::views::all(std::forward<Range>(range)));
		}
	} // namespace internal

	struct View {
		template <std::ranges::range Range>
		[[nodiscard]]
		static auto from(Range&& range) {
			return internal::makeView(std::forward<Range>(range));
		}
	};

	template <std::ranges::view V>
	class BasicView {
	private:
		V self;

	public:
		using iterator = std::ranges::iterator_t<V>;
		using sentinel = std::ranges::sentinel_t<V>;
		using reference = std::ranges::range_reference_t<V>;
		using value_type = std::ranges::range_value_t<V>;

		BasicView()
			requires std::default_initializable<V>
		= default;

		explicit BasicView(V self)
			: self(std::move(self)) {
		}

		[[nodiscard]]
		iterator begin() {
			return std::ranges::begin(self);
		}

		[[nodiscard]]
		sentinel end() {
			return std::ranges::end(self);
		}

		[[nodiscard]]
		auto begin() const
			requires std::ranges::range<const V>
		{
			return std::ranges::begin(self);
		}

		[[nodiscard]]
		auto end() const
			requires std::ranges::range<const V>
		{
			return std::ranges::end(self);
		}

		[[nodiscard]]
		auto cbegin() const
			requires std::ranges::range<const V>
		{
			return std::ranges::begin(self);
		}

		[[nodiscard]]
		auto cend() const
			requires std::ranges::range<const V>
		{
			return std::ranges::end(self);
		}

		[[nodiscard]]
		bool empty() {
			return std::ranges::empty(self);
		}

		[[nodiscard]]
		bool empty() const
			requires requires { std::ranges::empty(self); }
		{
			return std::ranges::empty(self);
		}

		template <class Pred>
		[[nodiscard]]
		bool any(Pred&& pred) const {
			return std::ranges::any_of(self, std::forward<Pred>(pred));
		}

		template <class Value, class Proj = std::identity>
		[[nodiscard]]
		bool contains(const Value& value, Proj proj = {}) const {
			return std::ranges::find(self, value, std::move(proj)) != std::ranges::end(self);
		}

		template <class Pred>
		[[nodiscard]]
		auto where(Pred pred) const {
			return internal::makeView(self | std::views::filter(std::move(pred)));
		}

		template <class Func>
		[[nodiscard]]
		auto select(Func func) const {
			return internal::makeView(self | std::views::transform(std::move(func)));
		}

		template <template <class...> class Container>
		[[nodiscard]]
		auto to() const {
			using T = std::ranges::range_value_t<V>;

			Container<T> result;
			std::ranges::copy(self, std::back_inserter(result));

			return result;
		}
	};

	template <std::ranges::range Range>
	BasicView(Range&&) -> BasicView<std::views::all_t<Range>>;
} // namespace flx