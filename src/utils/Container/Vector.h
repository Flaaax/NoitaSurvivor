#pragma once
#include <array>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace Util {
	// Wrapper for std::vector
	template <class T>
		requires(!std::is_same_v<T, bool>)
	class Vector : public std::vector<T> {
	public:
		using Base = std::vector<T>;
		using Base::Base;

		template <std::integral I>
		T& operator[](I index) {
			return Base::operator[](static_cast<size_t>(index));
		}

		template <std::integral I>
		const T& operator[](I index) const {
			return Base::operator[](static_cast<size_t>(index));
		}

		void fill(T elem) {
			for (size_t i = 0; i < this->size(); ++i) {
				this->operator[](i) = elem;
			}
		}

		operator Base&() {
			return static_cast<Base&>(*this);
		}

		operator const Base&() const {
			return static_cast<const Base&>(*this);
		}

		template <class U>
		void operator+=(U&& elem) {
			this->emplace_back(std::forward<U>(elem));
		}

		template <class... Args>
		T& emplace_front(Args&&... args) {
			auto it = this->emplace(
				this->begin(),
				std::forward<Args>(args)...);
			return *it;
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

		bool valid(int index) const {
			return index >= 0 && index < this->size();
		}

		auto indices() const {
			using size_type = typename Base::size_type;
			return std::views::iota(size_type{0}, this->size());
		}
	};

	template <typename T>
	concept IntegerOrEnum =
		std::integral<T> ||
		std::is_enum_v<T>;

	template <class T, std::size_t N>
	class Array : public std::array<T, N> {
	public:
		using Base = std::array<T, N>;
		using Base::Base;

		template <IntegerOrEnum I>
		T& operator[](I i) {
			return Base::operator[](static_cast<size_t>(i));
		}

		template <IntegerOrEnum I>
		const T& operator[](I i) const {
			return Base::operator[](static_cast<size_t>(i));
		}
	};

} // namespace Util