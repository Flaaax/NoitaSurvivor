#pragma once
#include <memory>

namespace flx {
	template <typename T>
	class Singleton {
	public:
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

		static T& inst() {
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

		static std::shared_ptr<T>& inst() {
			static std::shared_ptr<T> ins = std::shared_ptr<T>(new T(), std::default_delete<T>{});
			return ins;
		}

	protected:
		SharedSingleton() = default;
		virtual ~SharedSingleton() = default;
	};
} // namespace flx

#ifndef FLX_SINGLETON_DEF
#define FLX_SINGLETON_DEF

#define FLX_DEF_SINGLETON(T) class T : public ::flx::Singleton<T>

#define FLX_DECL_SINGLETON(Class) \
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