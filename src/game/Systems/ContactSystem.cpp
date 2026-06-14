#include "ContactSystem.h"

#include "../../utils/Logging/Logger.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Components/Script/ScriptComponent.h"
#include "src/game/Services/ContactService.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Services/PlayerService.h"

#include <boost/unordered/unordered_flat_map.hpp>

namespace flx::game {
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

	static void handleContactEvents(const GameCtx& ctx) {
		auto& reg = ctx.reg;

		for (auto& events = ctx.contactState.events; auto [pair, event] : events) {
			auto [a, b] = pair;

			if (!(EntityService().isAlive(ctx, a) && EntityService().isAlive(ctx, b))) {
				continue;
			}

			auto [ea, eb] = reg.get<EntityComponent>(a, b);
			if (ctx.contactRules.softContact.get(ea.layer, eb.layer)) {
				PhysicsService().applySoftCollision(ctx, a, b);
			}

			std::initializer_list<n_pair<entity>> pairs = {{a, b}, {b, a}};

			bool enabled = true;

			for (auto [a, b] : pairs) {
				// auto [ea, eb] = reg.get<EntityComponent>(a, b);
				auto [ba, bb] = reg.get<BodyComponent>(a, b);

				if (a == ctx.gameState.player.collector) {
					if (const auto mc = reg.try_get<MaterialComponent>(b)) {
						vec2 dir = PhysicsService().getPosition(ba) - PhysicsService().getPosition(bb);
						if (const float len = dir.normalize(); len < 0.5f) {
							EntityService::kill(ctx, b);
							PlayerService().gainMaterial(ctx, mc->value);
						} else {
							reg.get<DirectionComponent>(b).dir = dir;
						}
					}
					enabled = false;
				}
			}

			if (!enabled)
				continue;

			auto [mc1, mc2] = reg.try_get<MultiContactComponent>(a, b);

			if (ContactService().isBanned(mc1, b) || ContactService().isBanned(mc2, a)) {
				continue;
			}

			for (const auto [a, b] : pairs) {
				if (const auto sc = reg.try_get<ScriptComponent>(a); sc) {
					for (const auto& script : sc->scripts) {
						script->onContact(ctx, a, b, event);
					}
				}
			}

			for (auto [a, b] : pairs) {
				if (auto pa = reg.try_get<ProjectileComponent>(a); pa) {
					// Logger::info("Contact event: p = {}, other = {}", a.string(), b.string());

					const auto& otherBody = reg.get<BodyComponent>(b);
					auto& pc = reg.get<ProjectileComponent>(a);

					vec2 impulseDir{};
					if (reg.has<ExplosionComponent>(a)) {
						vec2 posA = PhysicsService().getPosition(ctx, a);
						vec2 posB = PhysicsService().getPosition(otherBody);
						impulseDir = (posB - posA).normalized();
					} else {
						impulseDir = PhysicsService().getVelocity(reg.get<BodyComponent>(a)).normalized();
					}

					// If target is not projectile, then don't apply impulse
					if (auto [p1, p2] = reg.try_get<ProjectileComponent>(a, b); !p2) {
						const vec2 impulseApplied = p1->impulse * impulseDir;
						PhysicsService().applyImpulse(otherBody, impulseApplied);
						if (const auto ec = reg.try_get<EnemyComponent>(b)) {
							ec->impulseRecieved += impulseApplied;
						}
					}

					if (pc.pierce > 0) {
						pc.pierce--;
					}
					if (pc.pierce == 0) {
						EntityService().kill(ctx, a);
					}

					if (auto [e, s] = reg.try_get<EntityComponent, SpellOnDeathComponent>(a); e && !EntityService::isAlive(ctx, a) && s) {
						const auto& bc = reg.get<BodyComponent>(a);
						s->impulseDir = impulseDir;
						// todo This could cause different behavior
						// s->impulsePosFix = -bc.getVelocity() * (dt * 1.5f);
					}

					// Deal damage
					EntityService().damage(ctx, a, b, pc.damage);

					break;
				}
			}

			ContactService().addMultiContact(mc1, b);
			ContactService().addMultiContact(mc2, a);
		}
	}

	void ContactSystem::handleEvents(const GameCtx& ctx) {
		handleContactEvents(ctx);
	}

	void ContactSystem::updateAfterHandleEvent(const GameCtx& ctx, float dt) {
		for (auto [e, mc] : ctx.reg.view<MultiContactComponent>()) {
			ContactService().updateMultiContacts(mc, dt);
		}
		ctx.contactState.events.clear();
	}
} // namespace flx::game