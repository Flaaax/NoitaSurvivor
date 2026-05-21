// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "PhysicalContactCallbacks.h"

#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/Script/ScriptComponent.h"
#include "src/game/GameContext.h"
#include "src/game/Services/ContactService.h"
#include "src/game/Services/EntityService.h"
#include "src/utils/Logger.h"

#include <box2d/box2d.h>

using namespace myecs;

bool PhysicalContactCallbacks::filter(const GameCtx& ctx, myecs::entity a, myecs::entity b) {
	n_pair<entity> pairs[2] = {{a, b}, {b, a}};
	auto& reg = ctx.reg;

	if (!(reg.valid(a) && reg.valid(b))) {
		Logger::error("A shape attached to invalid entity!");
		return true;
	}
	auto [ea, eb] = reg.try_get<EntityComponent>(a, b);

	// disable if one is not alive (Which is not likely to happen)
	if (!(ea && eb) || !(EntityService::isAlive(ctx, a) && EntityService::isAlive(ctx, b))) {
		Logger::error("Detected dead entity: {} or {}", a.string(), b.string());
		return false;
	}

	// disable for the basic contact rules
	// if (!ctx.contactRules.preSolve.get(ea->layer, eb->layer)) {
	// 	return false;
	// }

	// disable if in multicontact list
	for (auto& [a, b] : pairs) {
		if (ContactService().isBanned(reg.try_get<MultiContactComponent>(a), b)) {
			return false;
		}
	}

	return true;
}

bool PhysicalContactCallbacks::filterCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void* context) {
	const GameCtx& ctx = *static_cast<const GameCtx*>(context);
	auto [a, b] = ContactService().getEntityPair(shapeIdA, shapeIdB);
	return filter(ctx, a, b);
}

bool PhysicalContactCallbacks::presolveCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context) {
	const GameCtx& ctx = *static_cast<const GameCtx*>(context);
	auto [a, b] = ContactService().getEntityPair(shapeIdA, shapeIdB);
	auto& reg = ctx.reg;

	if (!(reg.valid(a) && reg.valid(b))) {
		Logger::warn("A shape attached to invalid entity!");
		return true;
	}

	ContactSettings settings{true, true};

	const auto layerA = EntityService().getLayer(ctx, a);
	const auto layerB = EntityService().getLayer(ctx, b);

	settings.enablePhysics = ctx.contactRules.physics.get(layerA, layerB);

	// else if (auto [ea, eb] = reg.try_get<EntityComponent>(a, b); ea->layer == Collector || eb->layer == Collector) {
	// 	settings.enablePhysics = false;
	// }
	// else if (ContactService().isBanned(mc1, b) || ContactService().isBanned(mc2, a)) {
	// 	settings.enablePhysics = false;
	// 	settings.emitEvent = false;
	// }

	std::initializer_list<n_pair<entity>> pairs = {{a, b}, {b, a}};

	for (auto& [a, b] : pairs) {
		if (const auto s = reg.try_get<ScriptComponent>(a)) {
			for (const auto& sc : s->scripts) {
				sc->configureContact(ctx, a, b, settings);
			}
		}
	}

	if (!settings.emitEvent) {
		return false;
	}

	auto& events = ctx.contactState.events;
	if (events.contains({a, b})) {
		return settings.enablePhysics;
	}

	events[{a, b}] = ContactEvent{
		.normal = manifold->normal,
		.isPhysicsEnabled = settings.enablePhysics,
		.isSensorEvent = false,
	};

	return settings.enablePhysics;
}
