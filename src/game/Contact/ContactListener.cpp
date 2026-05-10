#include"ContactListener.h"
#include"src/game/Components/EntityComponents.h"
#include"src/game/Components/PhysicsComponents.h"
#include"src/game/Components/Script/ScriptComponent.h"
#include"src/game/Services/PlayerService.h"

using namespace myecs;

static auto getContactEntity(b2Contact* contact) {
	myecs::entity a = myecs::entity(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	myecs::entity b = myecs::entity(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	return std::array{ a,b };
}

void GameContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	auto [a, b] = getContactEntity(contact);
	std::initializer_list<n_pair<entity>> pairs = { {a,b}, {b,a} };
	auto& reg = ctx.reg;

	auto ea = reg.try_get<EntityComponent>(a);
	auto eb = reg.try_get<EntityComponent>(b);

	//disable if one is not alive
	if (!(ea && eb) || !(ea->isAlive() && eb->isAlive())) {
		contact->SetEnabled(false);
		return;
	}

	//disable if in multicontact list
	for (auto& [a, b] : pairs) {
		if (auto c = reg.try_get<MultiContactComponent>(a); c && c->isBannedContact(b)) {
			contact->SetEnabled(false);
			return;
		}
	}

	//disable for the basic contact rules
	if (!ctx.contactRules.shouldContact(ea->layer, eb->layer)) {
		contact->SetEnabled(false);
		return;
	}

	//handle scriptss
	for (auto& [a, b] : pairs) {
		if (auto s = reg.try_get<ScriptComponent>(a)) {
			for (auto& sc : s->scripts) {
				sc->onContact(ctx, a, b, contact);
			}
		}
	}

	if (!contact->IsEnabled()) return;

	//handle if the entity is collectable
	if (contact->GetFixtureA()->GetFilterData().maskBits == (uint16)ContactMaskFilter::Collectable) {
		myecs::entity collector = ctx.state.player.collector;
		myecs::entity other = a == collector ? b : a;
		if (auto mc = reg.try_get<MaterialComponent>(other)) {
			auto& b1 = reg.get<BodyComponent>(collector);
			auto& b2 = reg.get<BodyComponent>(other);
			auto dir = (b1.getPosition() - b2.getPosition());
			float len = dir.normalize();
			if (len < 0.5f) {
				reg.get<EntityComponent>(other).kill();
				PlayerService().gainMaterial(ctx, mc->value);
			}
			else {
				reg.get<DirectionComponent>(other).dir = dir;
			}
		}
		contact->SetEnabled(false);
		return;
	}

	//handle multicontact
	for (auto& [a, b] : pairs) {
		if (auto m = reg.try_get<MultiContactComponent>(a)) {
			m->addContact(b);
		}
	}

	//currently, projectiles shouldn't contact each other
	auto onProjectileContact = [&](entity self, entity other) {
		b2Body* otherBody = reg.get<BodyComponent>(other).body;
		auto& pc = reg.get<ProjectileComponent>(self);

		std::optional<nvec2> _impluseDir;
		auto getImpulseDir = [&]() -> nvec2 {
			if (!_impluseDir) {
				b2WorldManifold manifold;
				contact->GetWorldManifold(&manifold);
				_impluseDir = (otherBody == contact->GetFixtureA()->GetBody()) ? manifold.normal : -manifold.normal;
			}
			return *_impluseDir;
		};

		//handle pierce
		if (pc.canPierce()) {
			contact->SetEnabled(false);

			//modify pierce count
			pc.doPierce();
			//not apply repluse if other is also pierce
			if (auto p = reg.try_get<ProjectileComponent>(other); p && p->canPierce()) {
				return;
			}

			//apply repulse if self is projectile
			if (auto pc = reg.try_get<ProjectileComponent>(self); pc && pc->impulse != 0) {
				nvec2 impulseRecieved = pc->impulse * -getImpulseDir();
				otherBody->ApplyLinearImpulseToCenter(impulseRecieved, true);
				if (auto ec = reg.try_get<EnemyComponent>(other)) {
					ec->impulse += impulseRecieved;
				}
			}

			if (!pc.canPierce()) {
				reg.get<EntityComponent>(self).kill();
			}
		}

		if (auto [e, s] = reg.try_get<EntityComponent, SpellOnDeathComponent>(self); (!e->isAlive()) && s) {
			auto& reg = ctx.reg;
			auto& b = reg.get<BodyComponent>(self);
			s->impulseDir = getImpulseDir();
			s->impulsePosFix = -b.getVelocity() * (ctx.debug_dt * 1.5f);
		}

		//deal damage
		reg.get<EntityComponent>(other).takeDamage(pc.damage);
	};

	if (reg.has<ProjectileComponent>(a)) {
		onProjectileContact(a, b);
	}
	else if (reg.has<ProjectileComponent>(b)) {		//warning projectiles shouldn't collide each other, for now
		onProjectileContact(b, a);
	}
}

void GameContactListener::BeginContact(b2Contact* contact) {
}
