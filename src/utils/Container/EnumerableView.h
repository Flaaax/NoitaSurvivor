#pragma once

#include "src/utils/Integers.h"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>

namespace Util {

	template <class Ref>
	struct IEnumerator {
		virtual ~IEnumerator() = default;

		virtual void next() = 0;
		[[nodiscard]] virtual Ref current() = 0;
		[[nodiscard]] virtual bool hasNext() = 0;
	};

	template <class Ref>
	struct IEnumerable {
		virtual ~IEnumerable() = default;

		[[nodiscard]] virtual std::unique_ptr<IEnumerator<Ref>> getEnumerator() = 0;
	};

	template <class Ref, class Iter, class Sent>
	struct DefaultEnumerator : IEnumerator<Ref> {
		Iter cur;
		Sent end;

		DefaultEnumerator(Iter cur, Sent end)
			: cur(std::move(cur)), end(std::move(end)) {
		}

		void next() override {
			++cur;
		}

		Ref current() override {
			return *cur;
		}

		[[nodiscard]] bool hasNext() override {
			return cur != end;
		}
	};

	template <class Ref, class Range>
	struct RangeEnumerable : IEnumerable<Ref> {
		Range self;

		explicit RangeEnumerable(Range&& self)
			: self(std::forward<Range>(self)) {
		}

		std::unique_ptr<IEnumerator<Ref>> getEnumerator() override {
			using Iter = decltype(self.begin());
			using Sent = decltype(self.end());

			return std::unique_ptr<IEnumerator<Ref>>(
				new DefaultEnumerator<Ref, Iter, Sent>(self.begin(), self.end()));
		}
	};

	namespace internal {
		struct NonType;
	};

	template <class T = internal::NonType, class Ref = T&>
		requires(!std::is_reference_v<T>)
	class EnumerableView {
	private:
		std::shared_ptr<IEnumerable<Ref>> enumerable{};

		template <bool IsConst>
		class Iterator {
		private:
			std::unique_ptr<IEnumerator<Ref>> enumerator{};

			void normalize() {
				if (!enumerator || !enumerator->hasNext()) {
					enumerator = {};
				}
			}

		public:
			using iterator_category = std::input_iterator_tag;
			using value_type = std::remove_cv_t<T>;
			using difference_type = std::ptrdiff_t;

			using reference = std::conditional_t<
				std::is_reference_v<Ref>,
				std::conditional_t<IsConst, const std::remove_cv_t<T>&, Ref>,
				std::remove_cv_t<T>>;

			using pointer = std::add_pointer_t<std::remove_reference_t<reference>>;

			Iterator() {
			}

			explicit Iterator(std::unique_ptr<IEnumerator<Ref>> e)
				: enumerator(std::move(e)) {
				normalize();
			}

			reference operator*() const {
				return enumerator->current();
			}

			pointer operator->() const
				requires std::is_reference_v<reference>
			{
				return std::addressof(operator*());
			}

			Iterator& operator++() {
				enumerator->next();
				normalize();
				return *this;
			}

			void operator++(int) {
				++*this;
			}

			friend bool operator==(const Iterator& it, std::default_sentinel_t) noexcept {
				return !it.enumerator;
			}

			friend bool operator==(std::default_sentinel_t, const Iterator& it) noexcept {
				return !it.enumerator;
			}
		};

	public:
		using iterator = Iterator<false>;
		using const_iterator = Iterator<true>;
		using sentinel = std::default_sentinel_t;

		EnumerableView() = default;

		explicit EnumerableView(std::shared_ptr<IEnumerable<Ref>> e)
			: enumerable(std::move(e)) {
		}

		EnumerableView(const EnumerableView& other) : enumerable(other.enumerable) {
		}

		template <class Range>
		static EnumerableView from(Range&& range) {
			using StoredRange = std::conditional_t<
				std::is_lvalue_reference_v<Range&&>,
				Range,
				std::remove_cvref_t<Range>>;

			auto e = std::shared_ptr<IEnumerable<Ref>>(
				new RangeEnumerable<Ref, StoredRange>(std::forward<Range>(range)));

			return EnumerableView(e);
		}

		iterator begin() {
			if (!enumerable) {
				return iterator{};
			}

			return iterator(enumerable->getEnumerator());
		}

		const_iterator begin() const {
			if (!enumerable) {
				return const_iterator{};
			}

			return const_iterator(enumerable->getEnumerator());
		}

		const_iterator cbegin() const {
			return begin();
		}

		static sentinel end() noexcept {
			return {};
		}

		static sentinel cend() {
			return end();
		}

		template <class Pred>
		[[nodiscard]]
		bool any(Pred&& pred) const {
			return std::ranges::any_of(*this, std::forward<Pred>(pred));
		}

		template <class Pred>
		[[nodiscard]]
		EnumerableView where(Pred pred) const {
			auto source = *this;

			auto filtered = std::views::filter(
				std::move(source),
				std::move(pred));

			return EnumerableView::from(std::move(filtered));
		}

		[[nodiscard]]
		bool empty() const {
			return begin() == end();
		}

		template <template <class...> class Container>
		[[nodiscard]]
		Container<std::remove_cv_t<T>> to() const {
			using Value = std::remove_cv_t<T>;

			Container<Value> result;
			std::ranges::copy(*this, std::back_inserter(result));

			return result;
		}

		template <class Func>
			requires std::invocable<Func&, Ref> &&
					 (!std::is_reference_v<std::invoke_result_t<Func&, Ref>>) &&
					 (!std::is_void_v<std::invoke_result_t<Func&, Ref>>)
		[[nodiscard]]
		auto select(Func func) const
			-> EnumerableView<
				std::remove_cv_t<std::invoke_result_t<Func&, Ref>>,
				std::remove_cv_t<std::invoke_result_t<Func&, Ref>>> {
			using Result = std::remove_cv_t<std::invoke_result_t<Func&, Ref>>;

			auto source = *this;

			auto transformed = std::views::transform(
				std::move(source),
				std::move(func));

			return EnumerableView<Result, Result>::from(std::move(transformed));
		}

		template <class Pred = std::identity>
			requires std::is_reference_v<Ref> &&
					 std::predicate<Pred&, Ref>
		[[nodiscard]]
		auto firstOrDefault(Pred pred = {})
			-> std::add_pointer_t<std::remove_reference_t<Ref>> {
			auto found = std::ranges::find_if(*this, std::move(pred));

			if (found == end()) {
				return nullptr;
			}

			return std::addressof(*found);
		}
	};

	template <>
	class EnumerableView<internal::NonType, internal::NonType&> {
	public:
		template <std::ranges::range Range>
		[[nodiscard]]
		static auto from(Range&& range) {
			using Ref = std::ranges::range_reference_t<Range&&>;
			using T = std::remove_cvref_t<Ref>;
			return EnumerableView<T, Ref>::from(std::forward<Range>(range));
		}
	};

	struct Enumerable {
		template <std::ranges::range Range>
		[[nodiscard]]
		static auto from(Range&& range) {
			using StoredRange = std::conditional_t<
				std::is_lvalue_reference_v<Range&&>,
				Range,
				std::remove_cvref_t<Range>>;

			using Ref = std::ranges::range_reference_t<StoredRange&>;
			using T = std::remove_cvref_t<Ref>;

			return EnumerableView<T, Ref>::from(std::forward<Range>(range));
		}
	};

	template <class T>
	using ValEnumerableView = EnumerableView<T, T>;

	template <class Derived, class T>
	struct Viewable {
		auto view() & {
			auto& self = static_cast<Derived&>(*this);

			using Ref = std::ranges::range_reference_t<Derived&>;
			using Elem = std::remove_cvref_t<Ref>;

			return EnumerableView<Elem, Ref>::from(self);
		}

		auto view() const& {
			const auto& self = static_cast<const Derived&>(*this);

			using Ref = std::ranges::range_reference_t<const Derived&>;
			using Elem = std::remove_cvref_t<Ref>;

			return EnumerableView<Elem, Ref>::from(self);
		}

		auto view() && = delete;
		auto view() const&& = delete;
	};

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
} // namespace Util

namespace std::ranges { // NOLINT(*-dcl58-cpp)
	template <class T, class Ref>
	inline constexpr bool enable_view<Util::EnumerableView<T, Ref>> = true;
}