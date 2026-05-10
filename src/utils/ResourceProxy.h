#pragma once
#include<forward_list>
#include<deque>
#include<any>


namespace Util {

	template<template <typename...> typename Container>
	class BaseResourceProxy {
	private:
		Container<std::any> storage;

		BaseResourceProxy(const BaseResourceProxy&) = delete;
	public:
		BaseResourceProxy() {}

		template<class T>
		T* add(T* data) {
			storage.emplace_front(std::unique_ptr<T>(data));
			return data;
		}

		template<class T, class ...Args>
		T* emplace(Args&&... args) {
			std::any& ret = storage.emplace_front(std::in_place_type<T>, std::forward<Args>(args)...);
			return std::any_cast<T>(&ret);
		}
	};

	using ResourceProxy = BaseResourceProxy<std::forward_list>;
	using BigResourceProxy = BaseResourceProxy<std::deque>;
}