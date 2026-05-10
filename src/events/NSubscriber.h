#pragma once

#ifndef NSUBSCRIBER_H
#define NSUBSCRIBER_H
#include"NEventListener.h"
#include"./args/EventArg.h"
#include<unordered_set>
#include<queue>


class INSubscriber {
protected:
	virtual ~INSubscriber() = default;

public:
	virtual void clear() = 0;
};


template<class T>
class NSubscriber :public INSubscriber {
private:
	using Listener = NEventListener<T>;

	std::unordered_set<std::shared_ptr<Listener>> listeners;

	std::vector<T> delayedEvents;

public:
	NSubscriber() {}

	void clear() override {
		listeners.clear();
		delayedEvents.clear();
	}

	void publish(const T& event, bool isInstant) {
		if (!isInstantPublish) {
			delayedEvents.emplace_back(event);
		}
		else {
			for (auto* listener : listeners) {
				if (eventType == listener->eventType) {
					listener->invoke(m_arg);
				}
			}
		}
	}

	void invokeDelayed() {
		for (auto& event : delayedEvents) {
			for (auto* listener : listeners) {
				if (event.type == listener->eventType) {
					listener->invoke(event);
				}
			}
		}
		delayedEvents.clear();
	}

	template<class Func>
	std::shared_ptr<Listener> create(Func&& func) {
		listeners.insert(std::make_shared<Listener>(std::forward<Func>(func)));
	}

	void subscribe(const std::shared_ptr<Listener>& listener) {
		listeners.insert(listener);
	}

	void unsubscribe(const std::shared_ptr<Listener>& listener) {
		listeners.erase(listener);
	}
};



class EventManager {
private:
	inline static size_t total_event_id = 0;

	template<class T>
	static size_t getEventId() {
		static size_t id = total_event_id++;
		return id;
	}

	std::vector<std::shared_ptr<INSubscriber>> subscribers;

	template<class T>
	NSubscriber<T>& getSubscriber() {
		auto id = getEventId<T>();
		if (subscribers.size() <= id) {
			subscribers.resize(id + 1);
		}
		auto& s = subscribers[id];
		if (!s) {
			s = std::make_shared<NSubscriber<T>>();
		}
		return *static_cast<NSubscriber<T>*>(s.get());
	}

public:
	template<class T, class Func>
	std::shared_ptr<NEventListener<T>> create(Func&& func) {
		auto& s = getSubscriber<T>();
		return s.create(std::forward<Func>(func));
	}

	template<class T>
	void subscribe(const std::shared_ptr<NEventListener<T>>& listener) {
		auto& s = getSubscriber<T>();
		s.subscribe(listener);
	}

	template<class T>
	void unsubscribe(const std::shared_ptr<NEventListener<T>>& listener) {
		auto& s = getSubscriber<T>();
		s.unsubscribe(listener);
	}

	template<class T>
	void clear() {
		auto& s = getSubscriber<T>();
		s.clear();
	}

	void clear() {
		for (auto& s : subscribers) {
			if (s) {
				s->clear();
			}
		}
	}

	template<class T>
	void publish(const T& event, bool isInstant) {
		auto& s = getSubscriber<T>();
		s.publish(event, isInstant);
	}

	template<class T>
	void invokeDelayed() {
		auto& s = getSubscriber<T>();
		s.invokeDelayed();
	}
};



#endif