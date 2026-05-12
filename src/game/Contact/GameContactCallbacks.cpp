#include "GameContactCallbacks.h"

#include "src/game/Components/EntityComponents.h"
#include "src/game/GameContext.h"
#include "src/game/Services/ContactService.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Logger.h"

#include <box2d/box2d.h>

using namespace myecs;

bool GameContactCallbacks::FilterCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void* context) {
	const GameCtx& ctx = *static_cast<GameCtx*>(context);
	auto [a, b] = ContactService().getEntitiesFromShapes(shapeIdA, shapeIdA);

	n_pair<entity> pairs[2] = {{a, b}, {b, a}};
	auto& reg = ctx.reg;

	if (!(reg.valid(a) && reg.valid(b))) {
		Logger::warn("A shape attached to invalid entity!");
		return true;
	}
	auto [ea, eb] = reg.try_get<EntityComponent>(a, b);

	// disable if one is not alive (Which is not likely to happen)
	if (!(ea && eb) || !(EntityService::isAlive(ctx, a) && EntityService::isAlive(ctx, b))) {
		Logger::warn("Detected dead entity: {} or {}", a.string(), b.string());
		return false;
	}

	// disable for the basic contact rules
	if (!ctx.contactRules.shouldContact(ea->layer, eb->layer)) {
		return false;
	}

	// disable if in multicontact list
	for (auto& [a, b] : pairs) {
		if (const auto mc = reg.try_get<MultiContactComponent>(a); mc && ContactService().isBanned(*mc, b)) {
			return false;
		}
	}

	return true;
}
