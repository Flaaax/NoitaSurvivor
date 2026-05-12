#include "PhysicsSystem.h"

#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"

#include <src/game/Components/EntityComponents.h>
#include <src/game/Components/PhysicsComponents.h>
#include <src/gui/NWindow.h>

using namespace myecs;

void PhysicsSystem::step(const GameCtx& ctx, float dt) {
	b2World_Step(ctx.worldCtx.world, dt, 4);
}

void PhysicsSystem::updateAfterContactSystem(const GameCtx& ctx, float dt) {
	// set material collector
	auto& reg = ctx.reg;
	PhysicsService ps{};
	ps.setPosition(ctx, ctx.gameState.player.collector, ctx.gameState.playerPos);

	for (const auto& [e, _, bc] : reg.view<ProjectileComponent, BodyComponent>()) {
		ps.setRotation(bc, Util::to_rad(ps.getVelocity(bc)));
	}

	// update enemy diretion
	for (const auto& [e, _, dc, bc] : reg.view<EnemyComponent, DirectionComponent, BodyComponent>()) {
		dc.dir = (ctx.gameState.playerPos - ps.getPosition(bc)).normalized();
	}

	// update player direction
	reg.get<DirectionComponent>(ctx.gameState.player.id).dir = ctx.gameState.player.controlDir;

	// update smoothmovement
	for (const auto& [e, sc, dc, bc] : reg.view<SmoothMovementComponent, DirectionComponent, BodyComponent>()) {
		nvec2 cur_dir = ps.getVelocity(bc);
		const float length = cur_dir.normalize();

		nvec2 force;
		if (length < sc.dec * dt) {
			// speed fix, very important in low frames
			ps.setVelocity(bc, {0, 0});
		} else {
			force += -cur_dir * sc.dec * bc.body->GetMass();
		}
		if (length < sc.spd) {
			force += dc.dir * (sc.acc + sc.dec) * bc.body->GetMass();
		}

		bc.body->ApplyForceToCenter(force, true);
	}

	// kill bodies out of the bound
	for (const auto& [e, bc] : reg.view<BodyComponent>()) {
		if (auto& bound = ctx.gameState.maxBound; !bound.contains(ps.getPosition(bc))) {
			// should be careful, I guess
			if (!reg.has<EntityComponent>(e))
				Logger::error_throw("Entity {} does not have BodyComponent", e.string());
			EntityService::kill(ctx, e);
			// Logger::info("Found entity {} out of bound",e.string());
		}
	}
}
