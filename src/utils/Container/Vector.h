#pragma once
#include "ContainerFeature.h"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

namespace flx {
	template <class T>
	using Span = std::span<T>;

	// Wrapper for std::vector
	template <class T>
		requires(!std::is_same_v<T, bool>)
	class Vector : public std::vector<T>, public ContainerFeature<Vector<T>, T, Indexable, Fillable> {
	public:
		using Base = std::vector<T>;
		using Base::Base;

		Vector() = default;

		explicit(false) Vector(Span<const T> span)
			: Base(span.begin(), span.end()) {}

		template <std::integral I>
		T& operator[](I index) {
			return Base::operator[](static_cast<size_t>(index));
		}

		template <std::integral I>
		const T& operator[](I index) const {
			return Base::operator[](static_cast<size_t>(index));
		}

		explicit(false) operator Base&() {
			return static_cast<Base&>(*this);
		}

		explicit(false) operator const Base&() const {
			return static_cast<const Base&>(*this);
		}

		template <std::convertible_to<T> U>
		void operator+=(U&& elem) {
			this->emplace_back(std::forward<U>(elem));
		}

		// Vector& operator=(Span<const T> span) {
		// 	this->assign(span.begin(), span.end());
		// 	return *this;
		// }

		template <class... Args>
		T& emplace_front(Args&&... args) {
			auto it = this->emplace(
				this->begin(),
				std::forward<Args>(args)...);
			return *it;
		}

		void pop_front() {
			this->erase(this->begin());
		}

		// Better: Returns true if left is better
		template <class Better>
			requires std::strict_weak_order<Better&, const T&, const T&>
		T& best(Better better) {
			if (this->empty()) {
				throw std::out_of_range("Util::Vector::best() called on empty Vector");
			}
			return *std::ranges::min_element(*this, std::ref(better));
		}

		template <class Compare = std::ranges::less, class Projection = std::identity>
			requires std::indirect_strict_weak_order<Compare, std::projected<std::ranges::iterator_t<Vector&>, Projection>>
		T& best(Compare compare, Projection projection) {
			if (this->empty()) {
				throw std::out_of_range("Util::Vector::best() called on empty Vector");
			}

			return *std::ranges::max_element(*this, std::move(compare), std::move(projection));
		}

		template <std::ranges::input_range R>
			requires std::convertible_to<std::ranges::range_reference_t<R>, T>
		void assign_range(R&& r) {
			this->clear();
			if constexpr (std::ranges::sized_range<R>) {
				this->reserve(std::ranges::size(r));
			}
			for (auto&& x : r) {
				this->emplace_back(x);
			}
		}
	};

	// Maybe move this to flx::trait
	template <typename T>
	concept IntegerOrEnum =
		std::integral<T> ||
		std::is_enum_v<T>;

	template <class T, std::size_t N>
	class Array : public std::array<T, N>, public ContainerFeature<Array<T, N>, T, Indexable> {
	public:
		using Base = std::array<T, N>;

		template <IntegerOrEnum I>
		T& operator[](I i) {
			return Base::operator[](static_cast<size_t>(i));
		}

		template <IntegerOrEnum I>
		const T& operator[](I i) const {
			return Base::operator[](static_cast<size_t>(i));
		}
	};

} // namespace flx