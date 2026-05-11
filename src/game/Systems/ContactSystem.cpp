#include "ContactSystem.h"

#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Components/Script/ScriptComponent.h"
#include "src/game/Services/PlayerService.h"

using namespace myecs;

template <class T>
struct Ref {
	entity e;
	T& data;
};

template <class T>
bool match_and_handle(entity a, T& dataA, entity b, T& dataB, auto&& valid, auto&& fn) {
	auto refa = Ref<T>{a, dataA};
	auto refb = Ref<T>{b, dataB};
	if (valid(refa, refb)) {
		fn(refa, refb);
		return true;
	} else if (valid(refb, refa)) {
		fn(refb, refa);
		return true;
	}
	return false;
}

void ContactSystem::update(GameCtx& ctx, float dt) {
	auto& reg = ctx.reg;

	//handle contact events
	for (const auto& event : ctx.contactState.events) {
		//handle if the entity is collectable
		entity a = event.a;
		entity b = event.b;
		//n_pair<entity> pairs[2] = {{a, b}, {b, a}};
		auto [ea,eb] = reg.get<EntityComponent>(a, b);

		if (!(ea.isAlive() && eb.isAlive())) {
			continue;
		}

		{
			using Ref = Ref<EntityComponent>;
			auto side = [&](Ref a, Ref b) {
				return a.e == ctx.gameState.player.collector && b.data.layer == Collectible;
			};
			auto fn = [&](Ref a, Ref b) {
				if (const auto mc = reg.try_get<MaterialComponent>(b.e)) {
					const auto& b1 = reg.get<BodyComponent>(a.e);
					const auto& b2 = reg.get<BodyComponent>(b.e);
					auto dir = b1.getPosition() - b2.getPosition();
					if (const float len = dir.normalize(); len < 0.5f) {
						b.data.kill();
						PlayerService().gainMaterial(ctx, mc->value);
					} else {
						reg.get<DirectionComponent>(b.e).dir = dir;
					}
				}
			};

			if (match_and_handle(a, ea, b, eb, side, fn)) {
				continue;
			}
		}

		std::initializer_list<n_pair<entity> > pairs = {{a, b}, {b, a}};

		//currently, projectiles shouldn't contact each other
		auto onProjectileContact = [&](entity self, entity other) {
			b2Body* otherBody = reg.get<BodyComponent>(other).body;
			const auto& pc = reg.get<ProjectileComponent>(self);

			const nvec2 impulseDir = other == a ? event.normal : -event.normal;

			//If target is not projectile, then don't apply impulse
			if (auto [p1,p2] = reg.try_get<ProjectileComponent>(self, other); !p2) {
				nvec2 impulseApplied = p1->impulse * -impulseDir;
				otherBody->ApplyLinearImpulseToCenter(impulseApplied, true);
				if (const auto ec = reg.try_get<EnemyComponent>(other)) {
					ec->impulse += impulseApplied;
				}
			}

			if (pc.pierce == 0) {
				reg.get<EntityComponent>(self).kill();
			}

			if (auto [e, s] = reg.try_get<EntityComponent, SpellOnDeathComponent>(self); (!e->isAlive()) && s) {
				const auto& bc = reg.get<BodyComponent>(self);
				s->impulseDir = impulseDir;
				s->impulsePosFix = -bc.getVelocity() * (dt * 1.5f);
			}

			//Deal damage
			//todo move this to a service
			reg.get<EntityComponent>(other).takeDamage(pc.damage);
		};

		//warning: projectiles shouldn't collide each other, for now

		if (reg.has<ProjectileComponent>(a)) {
			onProjectileContact(a, b);
		} else if (reg.has<ProjectileComponent>(b)) {
			onProjectileContact(b, a);
		}

		for (auto [a,b] : pairs) {
			if (const auto sc = reg.try_get<ScriptComponent>(a); sc) {
				for (const auto& script : sc->scripts) {
					script->onContact(ctx, event);
				}
			}
		}
	}

	//update Multi Contact
	for (const auto& [e, c] : reg.view<MultiContactComponent>()) {
		for (auto it = c.banned.begin(); it != c.banned.end();) {
			it->second.update(dt);
			if (!it->second.isRunning()) {
				it = c.banned.erase(it);
			} else {
				++it;
			}
		}
	}

	clearEvent(ctx);
}

void ContactSystem::clearEvent(GameCtx& ctx) {
	ctx.contactState.events.clear();
}