#include "LifeTimeSystem.h"
#include "src/ecs/entity.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/Render/SpriteEffects.h"
#include "src/game/GameContext.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Math.h"
#include "src/utils/Pointer.h"
#include <src/game/Components/PhysicsComponents.h>
#include <src/game/Components/Render/RenderComponent.h>
#include <src/game/Components/Script/ScriptComponent.h>
#include <src/game/Services/GameService.h>
#include <src/utils/Random.h>

namespace flx::game {
	void LifeTimeSystem::cleanupDeadEntities(const GameCtx& ctx) {
		auto& reg = ctx.reg;
		using namespace myecs;
		std::vector<entity> entityToDestroy;

		for (const auto& [e, c] : reg.view<EntityComponent>()) {
			if (!EntityService::isAlive(ctx, e)) {
				entityToDestroy.emplace_back(e);
			}
		}

		for (const entity& e : entityToDestroy) {
			if (!reg.has<DieSilentComponent>(e)) {
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
					ee.type = EntityType::None; // disable all contact
					ee.health = -1;
					PhysicsService().setType(body, BodyArg::Static);
					vec2 impulse = ec->impulseRecieved;
					reg.destroy<EnemyComponent>(e);
					// add death animation
					const float duration = flx::random.nextFloat(0.22f, 0.27f);
					const float k = flx::random.nextFloat(0.05f, 0.08f);
					const float darg = math::to_rad(flx::random.nextFloat(-30.f, 30.f));
					const float arg = flx::random.nextFloat(30.f, 120.f);
					const float spin = flx::random.nextBool() ? 1.f : -1.f;
					reg.emplace_or_replace<LifetimeComponent>(e).lifeTimer.start(duration);
					auto& sc = reg.get_or_emplace<SpriteEffectComponent>(e);
					sc.effectList += flx::makeUnique(new Tween(
						EffectState{},
						EffectState{
							.rotation = sf::degrees(spin * arg),
							.offset = (impulse * k).rotated(darg),
							.scale = {0.75, 0.75},
							.opacity = 0.f},
						duration));
					continue;
				}
			}

			if (reg.try_get<BodyComponent>(e)) {
				PhysicsService().destroyBody(ctx, e);
			}

			// Logger::info("Destroyed entity {}", e.string());
			reg.destroy(e);
		}
	}
} // namespace flx::game