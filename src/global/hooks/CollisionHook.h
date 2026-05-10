#pragma once
#include<box2d/box2d.h>
#include"src/global/EntityManager.h"


class CollisionHook :public  b2ContactListener {
private:
	//inline static CollisionManager& collisionManager = CollisionManager::instance();
	inline static EntityManager& entityManager = EntityManager::inst();

	struct EntityInfo {
		n_shared<Entity> A, B;
		const bool isValid;
		explicit EntityInfo(const n_shared<Entity>& _A, const n_shared<Entity>& _B) :
			A(_A), B(_B), isValid((_A) && (_B)) {
		}
		EntityInfo() :isValid(false) {}
	};

	EntityInfo getEntityInfo(b2Contact* contact) {
		if (!contact) {
			Logger::warn("Contact is nullptr");
			return {};
		}
		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		size_t idA = bodyA->GetUserData().pointer;
		size_t idB = bodyB->GetUserData().pointer;
		return EntityInfo(entityManager.queryActivatedEntity(idA), entityManager.queryActivatedEntity(idB));
	}
public:
	//sensor will not appear here
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {
		if (auto info = getEntityInfo(contact); info.isValid) {
			if (!Entity::canCollide(info.A, info.B)) {
				contact->SetEnabled(false);
			}
			else {
				info.A->onContact(info.B);
				info.B->onContact(info.A);
			}
		}
	}

	void BeginContact(b2Contact* contact) override {
		//both sensor or both not
		if (!contact->GetFixtureA()->IsSensor() && !contact->GetFixtureB()->IsSensor())return;
		if (auto info = getEntityInfo(contact); info.isValid && Entity::canCollide(info.A, info.B)) {
			info.A->onContact(info.B);
			info.B->onContact(info.A);
		}
	}

	//void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {
	//	if (auto info = getEntityInfo(contact); info.isValid) {
	//		/*if (!info.A->isDead()) {
	//			info.A->postContact();
	//		}
	//		if (!info.B->isDead()) {
	//			info.B->postContact();
	//		}*/
	//		auto* _impulse = const_cast<b2ContactImpulse*>(impulse);
	//		//*_impulse = {};
	//		_impulse->count = 0;
	//		_impulse->normalImpulses[0] = 0;
	//		Util::clearArray(_impulse->normalImpulses);
	//		Util::clearArray(_impulse->tangentImpulses);
	//		Logger::debug("cleared");
	//	}
	//}
};

