#include "LifeTimeSystem.h"
#include "src/ecs/entity.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/Render/SpriteEffects.h"
#include "src/game/GameContext.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Pointer.h"
#include <src/game/Components/PhysicsComponents.h>
#include <src/game/Components/Render/RenderComponent.h>
#include <src/game/Components/Script/ScriptComponent.h>
#include <src/game/Services/GameService.h>
#include <src/utils/Random.h>

void LifeTimeSystem::destroyDeadEntities(GameCtx& ctx) {
	auto& reg = ctx.reg;
	using namespace myecs;
	std::vector<entity> entityToDestroy;

	for (const auto& [e, c] : reg.view<EntityComponent>()) {
		if (!EntityService::isAlive(ctx, e)) {
			entityToDestroy.emplace_back(e);
		}
	}

	for (const entity& e : entityToDestroy) {
		if (auto [sc, bc] = reg.try_get<SpellOnDeathComponent, BodyComponent>(e); sc) {
			sc->spellBlock.cast(ctx, PhysicsService().getPosition(*bc) + sc->impulsePosFix, sc->impulsePosFix.rad());
		}
		if (const auto sc = reg.try_get<ScriptComponent>(e)) {
			for (const auto& s : sc->scripts) {
				s->onDeath(ctx, e);
			}
		}
		if (const auto ec = reg.try_get<EnemyComponent>(e)) {
			GameService().dropMaterial(ctx, PhysicsService().getPosition(ctx, e));
			// disable the enemy
			auto& body = reg.get<BodyComponent>(e);
			auto& ee = reg.get<EntityComponent>(e);
			ee.layer = None; // disable all contact
			ee.health = -1;
			PhysicsService().setType(body, BodyArg::Static);
			nvec2 impulse = ec->impulse;
			reg.destroy<EnemyComponent>(e);
			// add death animation
			const float duration = Util::random.getFloat(0.22, 0.27);
			const float k = Util::random.getFloat(0.03, 0.07);
			const float darg = Util::to_rad(Util::random.getFloat(-30, 30));
			const float arg = Util::random.getFloat(30.f, 120.f);
			const float spin = Util::random.getBool() ? 1.f : -1.f;
			reg.emplace_or_replace<LifetimeComponent>(e).lifeTimer.start(duration);
			auto& sc = reg.get_or_emplace<SpriteEffectComponent>(e);
			sc.effectList += Util::make_unique(new Transition(
				EffectState{},
				EffectState{
					.arg = spin * arg,
					.offset = (impulse * k).rotated(darg),
					.scale = {0.75, 0.75},
					.opacity = 0.f},
				duration));
			continue;
		}

		if (reg.try_get<BodyComponent>(e)) {
			PhysicsService().destroyBody(ctx, e);
		}

		// Logger::info("Destroyed entity {}", e.string());
		reg.destroy(e);
	}
}
