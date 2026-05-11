#include"ContactListener.h"
#include"src/game/Components/EntityComponents.h"
#include"src/game/Components/PhysicsComponents.h"
#include"src/game/Components/Script/ScriptComponent.h"
#include"src/game/Services/PlayerService.h"
#include "src/utils/Logger.h"

using namespace myecs;

static auto getContactEntity(b2Contact* contact) {
	const auto a = entity(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	const auto b = entity(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	return std::array{a, b};
}

void GameContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	auto [a, b] = getContactEntity(contact);
	std::initializer_list<n_pair<entity> > pairs = {{a, b}, {b, a}};
	auto& reg = ctx.reg;

	ContactSettings settings{true, true};

	auto [ea,eb] = reg.try_get<EntityComponent>(a, b);
	if (!(ea && eb)) {
		Logger::warn("Fixture not attached to an entity, does not emit event!");
		return;
	}

	//disable if one is not alive
	if (!(ea->isAlive() && eb->isAlive())) {
		contact->SetEnabled(false);
		return;
	}

	//handle scriptss
	for (auto& [a, b] : pairs) {
		if (const auto s = reg.try_get<ScriptComponent>(a)) {
			for (const auto& sc : s->scripts) {
				sc->modifyContactSettings(ctx, a, b, settings);
			}
		}
	}

	if (!settings.emitEvent) {
		contact->SetEnabled(false);
		return;
	}

	//Multi Contact
	auto [mc1,mc2] = reg.try_get<MultiContactComponent>(a, b);
	if ((mc1 && mc1->banned.contains(b)) || (mc2 && mc2->banned.contains(a))) {
		contact->SetEnabled(false);
		return;
	}

	//Handle piercing object
	auto [p1,p2] = reg.try_get<ProjectileComponent>(a, b);
	if ((p1 && p1->pierce == 0) || (p2 && p2->pierce == 0)) {
		contact->SetEnabled(false);
		return;
	}

	if ((p1 && p1->isBullet) || (p2 && p2->isBullet)) {
		settings.enablePhysics = false;
	}

	if (ea->layer == Collector || eb->layer == Collector) {
		settings.enablePhysics = false;
	}

	//Should contact at this point
	//Handle simple contact logic

	if (mc1)
		mc1->add(b);
	if (mc2)
		mc2->add(a);

	for (const auto p : {p1, p2}) {
		if (!p)
			continue;
		p->pierce--;
		if (p->pierce > 0) {
			settings.enablePhysics = false;
		}
	}

	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);

	ctx.contactState.events.push_back(ContactEvent{
		.a = a,
		.b = b,
		.normal = manifold.normal,
		.points = {manifold.points[0], manifold.points[1]},
		.isPhysicsEnabled = settings.enablePhysics,
	});

	contact->SetEnabled(settings.enablePhysics);
}

void GameContactListener::BeginContact(b2Contact* contact) {
}