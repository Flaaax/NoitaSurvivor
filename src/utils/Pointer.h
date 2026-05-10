#pragma once
#include<memory>
#include<array>

template<class T>
using n_shared = std::shared_ptr<T>;

template<class T>
using n_unique = std::unique_ptr<T>;

template<class T>
using n_weak = std::weak_ptr<T>;

template<class T>
using n_pair = std::array<T, 2>;

namespace Util {
	template<class T>
	inline n_shared<T> make_shared(T* ptr) {
		return n_shared<T>(ptr);
	}

	template<class T>
	inline n_unique<T> make_unique(T* ptr) {
		return n_unique<T>(ptr);
	}
}

