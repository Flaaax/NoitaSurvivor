//#pragma once
//#include"src/game/Entities/Projectiles/Projectile.h"
//#include"src/utils/Math.h"
//#include<functional>
//#include<memory>
//#include<SFML/Graphics.hpp>
//#include<vector>
//#include<iostream>
//
//
//class ProjectileHook :public Singleton<ProjectileHook> {
//private:
//	using callback_t = std::function<void(const n_shared<Projectile>&)>;
//	callback_t callBack;
//
//public:
//	void registerCallback(const callback_t& cb) {
//		callBack = cb;
//	}
//
//	void clear() {
//		callBack = nullptr;
//	}
//
//	void trigger(const n_shared<Projectile>& up) {
//		if (!callBack) {
//			throw std::runtime_error("class ProjectileHook: onTimeout_ not registered");
//		}
//		callBack(up);
//	}
//};

