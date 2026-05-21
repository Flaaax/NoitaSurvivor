#include "EntityService.h"

#include "src/game/Components/EntityComponents.h"

void EntityService::damage(const GameCtx& ctx, myecs::entity source, myecs::entity target, int damage) {
	auto& reg = ctx.reg;
	auto& ec = reg.get<EntityComponent>(target);

	if (ec.health <= 0)
		return;
	ec.health -= damage;
	if (ec.health <= 0) {
		ec.health = 0;
	}
}

void EntityService::kill(const GameCtx& ctx, myecs::entity e) {
	auto& [layer, health] = ctx.reg.get<EntityComponent>(e);
	health = 0;

	// todo Invoke death events...here?
}

void EntityService::killSilent(const GameCtx& ctx, myecs::entity e) {
	kill(ctx, e);
	ctx.reg.emplace<DieSilentComponent>(e);
}

bool EntityService::isAlive(const GameCtx& ctx, myecs::entity e) {
	auto& [layer, health] = ctx.reg.get<EntityComponent>(e);
	return health != 0;
}

bool EntityService::isValidAndAlive(const GameCtx& ctx, myecs::entity e) {
	return ctx.reg.valid(e) && isAlive(ctx, e);
}

ContactLayer EntityService::getLayer(const GameCtx& ctx, myecs::entity e) {
	return ctx.reg.get<EntityComponent>(e).layer;
}

void EntityService::clearMostEntities(const GameCtx& ctx) {
	auto es = Util::EnumerableView<>::from(ctx.reg.view<EntityComponent>()).to<Util::Vector>();
	for (auto& [e, ec] : es) {
		if (ec.layer == Enemy || ec.layer == Collectible || ec.layer == PlayerProjectile) {
			killSilent(ctx, e);
		}
	}
}