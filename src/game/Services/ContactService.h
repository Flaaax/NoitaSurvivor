#pragma once
#include "src/game/Components/EntityComponents.h"
#include "src/game/States/ContactState.h"
#include <box2d/box2d.h>

class ContactService {
public:
	// The return pair is sorted
	static EntityPair getEntityPair(b2ShapeId sa, b2ShapeId sb) {
		using namespace myecs;
		const auto a = entity(reinterpret_cast<u64>(b2Shape_GetUserData(sa)));
		const auto b = entity(reinterpret_cast<u64>(b2Shape_GetUserData(sb)));
		return {a, b};
	}

	static myecs::entity getEntity(b2ShapeId shape) {
		using namespace myecs;
		return entity(reinterpret_cast<u64>(b2Shape_GetUserData(shape)));
	}

	static void addMultiContact(MultiContactComponent* mc, myecs::entity target, float duration) {
		if (!mc)
			return;
		mc->banned[target].set(duration).start();
	}

	static void addMultiContact(MultiContactComponent* mc, myecs::entity target) {
		if (!mc)
			return;
		addMultiContact(mc, target, mc->default_duration);
	}

	static void updateMultiContacts(MultiContactComponent& mc, float dt) {
		for (auto it = mc.banned.begin(); it != mc.banned.end();) {
			it->second.update(dt);
			if (!it->second.isRunning()) {
				it = mc.banned.erase(it);
			} else {
				++it;
			}
		}
	}

	static bool isBanned(const MultiContactComponent* mc, myecs::entity e) {
		return mc && mc->banned.contains(e);
	}
};