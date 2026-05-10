#pragma once
#pragma once
#include<box2d/box2d.h>
#include"src/ecs/entity.h"
#include"src/game/GameContext.h"

class GameContactListener :public b2ContactListener {
private:
	GameCtx ctx;

public:
	GameContactListener(GameCtx ctx) :ctx(ctx) {}

	//sensor will not appear here
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

	void BeginContact(b2Contact* contact) override;

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

