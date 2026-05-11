#include "ContactFilter.h"
#include<box2d/box2d.h>
#include"src/utils/Pointer.h"
#include <src/game/Components/EntityComponents.h>
#include"src/utils/Logger.h"


using namespace myecs;

bool GameContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) {
	auto a = entity(fixtureA->GetBody()->GetUserData().pointer);
	auto b = entity(fixtureB->GetBody()->GetUserData().pointer);
	n_pair<entity> pairs[2] = {{a, b}, {b, a}};
	auto& reg = ctx.reg;

	if (!(reg.valid(a) && reg.valid(b))) {
		Logger::warn("A fixture does not have an entity!");
		return true;
	}
	auto ea = reg.try_get<EntityComponent>(a);
	auto eb = reg.try_get<EntityComponent>(b);

	//disable if one is not alive
	if (!(ea && eb) || !(ea->isAlive() && eb->isAlive())) {
		return false;
	}

	//disable for the basic contact rules
	if (!ctx.contactRules.shouldContact(ea->layer, eb->layer)) {
		return false;
	}

	//disable if in multicontact list
	for (auto& [a, b] : pairs) {
		if (const auto c = reg.try_get<MultiContactComponent>(a); c && c->banned.contains(b)) {
			return false;
		}
	}
	
	return true;
}