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
	myecs::entity proj{};
	myecs::entity target{};
	nvec2 offset;
	float lengthSquared;
	float maxForceSize = 15.f;

	CircleTrackingDetectorScript(const GameCtx& ctx, myecs::entity self, myecs::entity proj) {
		auto& reg = ctx.reg;
		this->proj = proj;
		const auto& self_body = reg.get<BodyComponent>(self);
		const auto& proj_body = reg.get<BodyComponent>(proj);
		PhysicsService ps{};
		ps.setPosition(self_body, ps.getPosition(proj_body));
	}

	bool killIfInvalid(const GameCtx& ctx, myecs::entity self) {
		if (isDone) {
			return true;
		}
		if (!EntityService::isValidAndAlive(ctx, proj)) {
			EntityService::kill(ctx, self);
			isDone = true;
			return true;
		}
		return false;
	}

	void onUpdate(const GameCtx& ctx, myecs::entity self, float dt) override {
		if (killIfInvalid(ctx, self)) {
			return;
		}
		PhysicsService ps{};

		auto& reg = ctx.reg;
		const auto& self_body = reg.get<BodyComponent>(self);
		const auto& proj_body = reg.get<BodyComponent>(proj);
		ps.setPosition(self_body, ps.getPosition(proj_body));

		if (!EntityService().isValidAndAlive(ctx, target)) {
			return;
		}

		const nvec2 dir = ps.getVelocity(proj_body).normalized();
		const nmat22 rot = {dir.y, -dir.x, dir.x, dir.y};
		const nvec2 rotatedOffset = rot * offset;

		// Target is in front or behind of the projectile
		if (std::abs(rotatedOffset.x) < nmath::n_epsilon) {
			return;
		}

		const float radius = rotatedOffset.lengthSquared() / (2.f * std::abs(rotatedOffset.x));
		const nvec2 forceDir = nvec2{dir.y, -dir.x} * (rotatedOffset.x > 0.f ? 1.f : -1.f);
		float forceSize = ps.getVelocity(proj_body).lengthSquared() * ps.getMass(proj_body) / radius;
		forceSize = std::min(forceSize, maxForceSize);
		ps.applyForce(proj_body, forceDir * forceSize);

		target = {};
	}

	void onContact(const GameCtx& ctx, myecs::entity self, myecs::entity other, const ContactEvent& event) override {
		if (killIfInvalid(ctx, self)) {
			return;
		}
		auto& reg = ctx.reg;
		PhysicsService ps{};
		if (!EntityService().isValidAndAlive(ctx, target)) {
			target = other;
			offset = ps.getPosition(ctx, other) - ps.getPosition(ctx, self);
			lengthSquared = offset.lengthSquared();
		} else {
			const nvec2 offset1 = ps.getPosition(ctx, other) - ps.getPosition(ctx, self);
			const float lengthSquared1 = offset1.lengthSquared();
			if (lengthSquared1 < lengthSquared) {
				target = other;
				offset = offset1;
				lengthSquared = lengthSquared1;
			}
		}
	}

	void configureContact(const GameCtx& ctx, myecs::entity self, myecs::entity other, ContactSettings& settings) override {
		// settings.emitEvent = true;
		settings.enablePhysics = false;
	}
};
