#pragma once

#ifndef NOBSERVER_H
#define NOBSERVER_H
#include<functional>



template<class T>
class NEventListener {
public:
	std::function<void(const T&)> onEvent;

	NEventListener(std::nullptr_t) {
		static_assert("Can't set listener to nullptr");
	}
	template<class Func>
	NEventListener(Func&& onEvent) : onEvent(onEvent) {}
	NEventListener(const std::function<void(const T&)>& onEvent) :onEvent(onEvent) {}
	NEventListener(NEventListener&&) = delete;

	void invoke(const T& event) const {
		onEvent(event);
	}
};


#endif