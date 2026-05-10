#include "EntityDestroySystem.h"
#include"src/game/GameContext.h"
#include"src/ecs/entity.h"
#include"src/game/Components/EntityComponents.h"
#include <src/game/Components/PhysicsComponents.h>
#include <src/game/Components/Script/ScriptComponent.h>
#include <src/game/Services/GameService.h>
#include <src/utils/Random.h>
#include <src/game/Components/Render/RenderComponent.h>
#include"src/utils/Pointer.h"
#include"src/game/Components/Render/SpriteEffects.h"
#include"src/game/Services/PhysicsBodyService.h"


void EntityDestroySystem::destroyDeadEntities(GameCtx& ctx) {
	auto& reg = ctx.reg;
	using namespace myecs;
	std::vector<entity> entityToDestroy;

	for (auto [e, c] : reg.view<EntityComponent>()) {
		if (!c.isAlive()) {
			entityToDestroy.emplace_back(e);
		}
	}

	for (entity e : entityToDestroy) {
		if (auto [sc, bc] = reg.try_get<SpellOnDeathComponent, BodyComponent>(e); sc) {
			sc->spellBlock.cast(ctx, bc->getPosition() + sc->impulsePosFix, sc->impulsePosFix.rad());
		}
		if (auto sc = reg.try_get<ScriptComponent>(e)) {
			for (auto& s : sc->scripts) {
				s->onDeath(ctx, e);
			}
		}
		if (auto ec = reg.try_get<EnemyComponent>(e)) {
			GameService().dropMaterial(reg.get<BodyComponent>(e).getPosition(), ctx);
			//disable the enemy
			auto& body = reg.get<BodyComponent>(e);
			auto& ee = reg.get<EntityComponent>(e);
			ee.layer = None;		//disable all contact
			ee.health = -1;
			body.body->SetType(b2_staticBody);
			nvec2 impulse = ec->impulse;
			reg.destroy<EnemyComponent>(e);
			//add death animation
			float duration = Util::random.getFloat(0.22, 0.27);
			float k = Util::random.getFloat(0.03, 0.07);
			float darg = Util::to_rad(Util::random.getFloat(-30, 30));
			float arg = Util::random.getFloat(30.f, 120.f);
			float spin = Util::random.getBool() ? 1.f : -1.f;
			reg.emplace_or_replace<LifetimeComponent>(e).lifeTimer.start(duration);
			auto& sc = reg.get_or_emplace<SpriteEffectComponent>(e);
			sc.effectList += Util::make_unique(new Transition(
				EffectState{},
				EffectState{
					.arg = spin * arg,
					.offset = (impulse * k).rotated(darg),
					.scale = {0.75, 0.75},
					.opacity = 0.f
				},
				duration
			));
			continue;
		}

		if (auto bc = reg.try_get<BodyComponent>(e)) {
			PhysicsBodyService().destroyBody(ctx, e);
		}

		reg.destroy(e);
	}
}
