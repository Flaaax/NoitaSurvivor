#pragma once
#include "../EntityComponents.h"
#include "ScriptComponent.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Mat22.h"
#include <unordered_set>

// class AccelerateTrackingScript :public Script {
// public:
//	struct Contact {
//		myecs::entity other;
//		nvec2 shift;
//		float lengthSquared;
//	};
//	myecs::entity proj;
//	myecs::entity target = myecs::null_entity;
//	std::vector<Contact> contacts;
//
//	AccelerateTrackingScript(myecs::entity detector, myecs::entity proj) {
//		auto& reg = Game::getRegistry();
//		this->proj = proj;
//		auto& s = reg.get_or_emplace<SmoothMovementComponent>(proj);
//		s.spd = reg.get<ProjectileComponent>(proj).maxSpeed;
//		s.acc = 200.f;
//		s.dec = 200.f;
//		auto& self_body = reg.get<BodyComponent>(detector);
//		auto& proj_body = reg.get<BodyComponent>(proj);
//		self_body.setPosition(proj_body.getPosition());
//	}
//
//	void onUpdate(myecs::entity self, float dt)override {
//		auto& reg = Game::getRegistry();
//		if (!reg.valid(proj) || !reg.get<EntityComponent>(proj).isAlive()) {
//			reg.get<EntityComponent>(self).kill();
//			isDone = true;
//			return;
//		}
//		auto& self_body = reg.get<BodyComponent>(self);
//		auto& proj_body = reg.get<BodyComponent>(proj);
//		self_body.setPosition(proj_body.getPosition());
//
//		auto [s, d] = reg.get<SmoothMovementComponent, DirectionComponent>(proj);
//
//		if (!reg.valid(target)) {
//			target = myecs::null_entity;
//			reg.get<MultiContactComponent>(self).disabled.clear();
//
//		}
//		if (target != myecs::null_entity) {
//			auto& target_body = reg.get<BodyComponent>(target);
//			d.dir = (target_body.getPosition() - self_body.getPosition()).normalized();
//			return;
//		}
//		if (contacts.empty()) {
//			d.dir = proj_body.getVelocity().normalized();
//			return;
//		}
//		auto it = std::max_element(contacts.begin(), contacts.end(), [](const auto& a, const auto& b) {
//			return a.lengthSquared < b.lengthSquared;
//		});
//		if (it->lengthSquared < nmath::n_epsilon_2) {
//			d.dir = {};
//		}
//		else {
//			d.dir = it->shift / std::sqrt(it->lengthSquared);
//		}
//		contacts.clear();
//		reg.get<MultiContactComponent>(self).disabled.clear();
//		target = it->other;
//	}
//
//	void onContact(myecs::entity self, myecs::entity other, b2Contact* contact)override {
//		contact->SetEnabled(false);
//		if (target != myecs::null_entity) {
//			return;
//		}
//		auto& reg = Game::getRegistry();
//		auto& self_body = reg.get<BodyComponent>(self);
//		auto& other_body = reg.get<BodyComponent>(other);
//		auto shift = other_body.getPosition() - self_body.getPosition();
//		contacts.push_back(Contact{
//			.other = other,
//			.shift = shift,
//			.lengthSquared = shift.lengthSquared()
//						   });
//	}
//
//	void onDeath(myecs::entity self)override {
//		auto& reg = Game::getRegistry();
//		if (auto ec = reg.try_get<EntityComponent>(self)) {
//			ec->kill();
//		}
//	}
// };

// todo fix the name of this class, the owner of the script
class CircleTrackingDetectorScript : public Script {
public:
	struct Contact {
		myecs::entity other;
		nvec2 shift;
		float lengthSquared;
	};

	myecs::entity proj{};
	myecs::entity target{};
	std::vector<Contact> contacts;
	std::unordered_set<myecs::entity> entities;
	bool hasTarget = false;

	CircleTrackingDetectorScript(const GameCtx& ctx, myecs::entity self, myecs::entity proj) {
		auto& reg = ctx.reg;
		this->proj = proj;
		const auto& self_body = reg.get<BodyComponent>(self);
		const auto& proj_body = reg.get<BodyComponent>(proj);
		PhysicsService ps{};
		ps.setPosition(self_body, ps.getPosition(proj_body));
	}

	void onUpdate(const GameCtx& ctx, myecs::entity self, float dt) override {
		auto& reg = ctx.reg;
		if (!reg.valid(proj) || !EntityService::isAlive(ctx, proj)) {
			EntityService::kill(ctx, self);
			isDone = true;
			return;
		}
		entities.clear();
		auto& self_body = reg.get<BodyComponent>(self);
		auto& proj_body = reg.get<BodyComponent>(proj);
		PhysicsService ps{};
		ps.setPosition(self_body, ps.getPosition(proj_body));

		if (!reg.valid(target)) {
			target = {};
		}

		nvec2 shift;
		if (!target.is_null() && hasTarget) {
			const auto& target_body = reg.get<BodyComponent>(target);
			shift = ps.getPosition(target_body) - ps.getPosition(self_body);
		} else if (!contacts.empty()) {
			const auto it =
				std::ranges::max_element(contacts, [](const Contact& a, const Contact& b) {
					return a.lengthSquared < b.lengthSquared;
				});
			shift = it->shift;
			target = it->other;
			contacts.clear();
		} else
			return;

		auto dir = ps.getVelocity(proj_body).normalized();
		nmat22 rot = {dir.y, -dir.x, dir.x, dir.y};
		nvec2 newShift = rot * shift;
		if (std::abs(newShift.x) < nmath::n_epsilon) {
			return;
		}
		float radius = newShift.lengthSquared() / (2.f * std::abs(newShift.x));
		nvec2 forceDir = nvec2{dir.y, -dir.x} * (newShift.x > 0.f ? 1.f : -1.f);
		float forceSize = ps.getVelocity(proj_body).lengthSquared() * ps.getMass(proj_body) / radius;
		forceSize = std::min(forceSize, 15.f);
		ps.applyForce(proj_body, forceDir * forceSize);
		hasTarget = false;
	}

	void modifyContactSettings(const GameCtx& ctx, myecs::entity self, myecs::entity other, ContactSettings& settings) override {
		settings.emitEvent = false;
		settings.enablePhysics = false;

		if (!target.is_null()) {
			if (other == target) {
				hasTarget = true;
			}
			return;
		}
		if (entities.contains(other)) {
			return;
		}
		auto& reg = ctx.reg;
		const auto& self_body = reg.get<BodyComponent>(self);
		const auto& other_body = reg.get<BodyComponent>(other);
		PhysicsService ps{};
		const auto shift = ps.getPosition(other_body) - ps.getPosition(self_body);
		contacts.push_back(Contact{
			.other = other,
			.shift = shift,
			.lengthSquared = shift.lengthSquared(),
		});
	}

	void onDeath(const GameCtx& ctx, myecs::entity self) override {
		EntityService::kill(ctx, self);
	}
};
