#pragma once
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
	};
} // namespace Util