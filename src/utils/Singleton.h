#pragma once
#include <memory>

template <typename T>
class Singleton {
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	inline static T& inst() {
		static T ins;
		return ins;
	}

protected:
	Singleton() = default;
	virtual ~Singleton() = default;
};

template <typename T>
class SharedSingleton {
public:
	SharedSingleton(const SharedSingleton&) = delete;
	SharedSingleton& operator=(const SharedSingleton&) = delete;

	inline static std::shared_ptr<T>& inst() {
		static std::shared_ptr<T> ins = std::shared_ptr<T>(new T(), std::default_delete<T>{});
		return ins;
	}

protected:
	SharedSingleton() = default;
	virtual ~SharedSingleton() = default;
};

#ifndef N_SINGLETON_DEF
#define N_SINGLETON_DEF

#define N_DEF_SINGLETON(T) class T : public Singleton<T>

#define N_SHARED_SINGLETON(x)             \
	friend class std::shared_ptr<x>;      \
	friend struct std::default_delete<x>; \
	friend class SharedSingleton<x>;

#define N_SINGLETON(x) friend class Singleton<x>;

#define N_DECL_SINGLETON(Class)   \
public:                           \
	inline static Class& inst() { \
		static Class ins;         \
		return ins;               \
	}                             \
                                  \
private:                          \
	Class(const Class&) = delete; \
	Class& operator=(const Class&) = delete


#endif