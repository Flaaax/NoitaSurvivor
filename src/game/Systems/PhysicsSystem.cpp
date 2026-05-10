#include"PhysicsSystem.h"
#include <src/game/Components/PhysicsComponents.h>
#include <src/gui/NWindow.h>
#include <src/game/Components/EntityComponents.h>
#include<iostream>

using namespace myecs;
void PhysicsSystem::update(float dt, GameCtx& ctx) {

	ctx.world.Step(dt, 3, 3);

	//set material collector
	auto& reg = ctx.reg;
	reg.get<BodyComponent>(ctx.state.player.collector).setPosition(ctx.state.playerPos);;

	for (const auto& [e, _, c] : reg.view<ProjectileComponent, BodyComponent>()) {
		c.body->SetTransform(c.getPosition(), Util::to_rad(c.getVelocity()));
	}

	//update enemy diretion
	for (const auto& [e, _, dc, bc] : reg.view<EnemyComponent, DirectionComponent, BodyComponent>()) {
		dc.dir = (ctx.state.playerPos - bc.getPosition()).normalized();
	}

	//update player direction
	reg.get<DirectionComponent>(ctx.state.player.id).dir = ctx.state.player.controlDir;

	//update smoothmovement
	for (const auto& [e, sc, dc, bc] : reg.view<SmoothMovementComponent, DirectionComponent, BodyComponent>()) {
		nvec2 cur_dir = bc.getVelocity();
		float length = cur_dir.normalize();

		nvec2 force;
		if (length < sc.dec * dt) {
			//speed fix, very important in low frames
			bc.setVelocity({ 0,0 });
		}
		else {
			force += -cur_dir * sc.dec * bc.body->GetMass();
		}
		if (length < sc.spd) {
			force += dc.dir * (sc.acc + sc.dec) * bc.body->GetMass();
		}
		bc.body->ApplyForceToCenter(force, true);
	}

	//update multicontact
	for (const auto& [e, c] : reg.view<MultiContactComponent>()) {
		for (auto it = c.disabled.begin(); it != c.disabled.end();) {
			it->second.update(dt);
			if (!it->second.isRunning()) {
				it = c.disabled.erase(it);
			}
			else ++it;
		}
	}

	//kill bodies out of the bound
	for (const auto& [e, bc] : reg.view<BodyComponent>()) {
		if (auto& bound = ctx.state.maxBound; !bound.contains(bc.getPosition())) {
			//should be careful, I guess
			if (!reg.has<EntityComponent>(e))throw std::runtime_error("this should never happen...");
			reg.get<EntityComponent>(e).kill();
			//Logger::info("Found entity {} out of bound",e.string());
		}
	}

}
