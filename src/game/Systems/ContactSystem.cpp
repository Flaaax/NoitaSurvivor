#include "ContactSystem.h"

#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Components/Script/ScriptComponent.h"
#include "src/game/Services/ContactService.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Services/PlayerService.h"

#include <boost/unordered/unordered_flat_map.hpp>
#include <span>

using namespace myecs;

template <class T> struct Ref {
	entity e;
	T& data;
};

template <class T> bool match_and_handle(entity a, T& dataA, entity b, T& dataB, auto&& valid, auto&& fn) {
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

void ContactSystem::handleContactEvent(GameCtx& ctx) {
	const b2ContactEvents allEvents = b2World_GetContactEvents(ctx.worldCtx.world);
	auto& reg = ctx.reg;

	boost::unordered_flat_map<std::pair<u64, u64>, bool> handledPairs{};

	for (const auto& event : std::span(allEvents.beginEvents, allEvents.beginCount)) {

		auto [a, b] = ContactService().getEntitiesFromShapes(event.shapeIdA, event.shapeIdB);

		if (bool& handled = handledPairs[std::minmax(a._entity, b._entity)]; !handled) {
			handled = true;
		} else {
			continue;
		}

		auto [ea, eb] = reg.get<EntityComponent>(a, b);

		if (!(EntityService().isAlive(ctx, a) && EntityService().isAlive(ctx, b))) {
			continue;
		}

		{
			using Ref = Ref<EntityComponent>;
			auto side = [&](Ref ra, Ref rb) { return ra.e == ctx.gameState.player.collector && rb.data.layer == Collectible; };
			auto fn = [&](Ref ra, Ref rb) {
				if (const auto mc = reg.try_get<MaterialComponent>(rb.e)) {
					const auto& b1 = reg.get<BodyComponent>(ra.e);
					const auto& b2 = reg.get<BodyComponent>(rb.e);
					nvec2 dir = PhysicsService().getPosition(b1) - PhysicsService().getPosition(b2);
					if (const float len = dir.normalize(); len < 0.5f) {
						EntityService::kill(ctx, rb.e);
						PlayerService().gainMaterial(ctx, mc->value);
					} else {
						reg.get<DirectionComponent>(rb.e).dir = dir;
					}
				}
			};

			if (match_and_handle(a, ea, b, eb, side, fn)) {
				// Collectibles
				continue;
			}
		}

		std::initializer_list<n_pair<entity>> pairs = {{a, b}, {b, a}};

		for (auto [a,b]:pairs) {

		}

		// currently, projectiles shouldn't contact each other
		auto onProjectileContact = [&](entity self, entity other) {
			const auto& otherBody = reg.get<BodyComponent>(other);
			const auto& pc = reg.get<ProjectileComponent>(self);

			const nvec2 impulseDir = other == a ? event.manifold.normal : -event.manifold.normal;

			// If target is not projectile, then don't apply impulse
			if (auto [p1, p2] = reg.try_get<ProjectileComponent>(self, other); !p2) {
				const nvec2 impulseApplied = p1->impulse * -impulseDir;
				PhysicsService().applyLinearImpulse(otherBody, impulseApplied);
				if (const auto ec = reg.try_get<EnemyComponent>(other)) {
					ec->impulse += impulseApplied;
				}
			}

			if (pc.pierce == 0) {
				EntityService().kill(ctx, self);
			}

			if (auto [e, s] = reg.try_get<EntityComponent, SpellOnDeathComponent>(self);
				e && !EntityService::isAlive(ctx, self) && s) {
				const auto& bc = reg.get<BodyComponent>(self);
				s->impulseDir = impulseDir;
				// todo This could cause different behavior
				// s->impulsePosFix = -bc.getVelocity() * (dt * 1.5f);
			}

			// Deal damage
			EntityService().damage(ctx, self, other, pc.damage);
		};

		// warning: projectiles shouldn't collide each other, for now

		if (reg.has<ProjectileComponent>(a)) {
			onProjectileContact(a, b);
		} else if (reg.has<ProjectileComponent>(b)) {
			onProjectileContact(b, a);
		}

		for (auto [a, b] : pairs) {
			if (const auto sc = reg.try_get<ScriptComponent>(a); sc) {
				for (const auto& script : sc->scripts) {
					script->onContact(ctx, event);
				}
			}
		}
	}
}

void ContactSystem::updateAfterHandleEvent(const GameCtx& ctx, float dt) {
	for (auto [e, mc] : ctx.reg.view<MultiContactComponent>()) {
		ContactService().updateMultiContacts(mc, dt);
	}
}
