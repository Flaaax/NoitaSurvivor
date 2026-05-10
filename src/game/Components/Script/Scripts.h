#pragma once
#include"ScriptComponent.h"
#include"../EntityComponents.h"
#include"src/game/Components/PhysicsComponents.h"
#include"src/utils/Mat22.h"
#include<unordered_set>


//class AccelerateTrackingScript :public Script {
//public:
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
//};


class CircleTrackingScript :public Script {
public:
	struct Contact {
		myecs::entity other;
		nvec2 shift;
		float lengthSquared;
	};
	myecs::entity proj;
	myecs::entity target = myecs::null_entity;
	std::vector<Contact> contacts;
	std::unordered_set<myecs::entity> entities;
	bool hasTarget = false;

	CircleTrackingScript(GameCtx& ctx, myecs::entity detector, myecs::entity proj) {
		auto& reg = ctx.reg;
		this->proj = proj;
		auto& self_body = reg.get<BodyComponent>(detector);
		auto& proj_body = reg.get<BodyComponent>(proj);
		self_body.setPosition(proj_body.getPosition());
	}

	void onUpdate(GameCtx& ctx, myecs::entity self, float dt)override {
		auto& reg = ctx.reg;
		if (!reg.valid(proj) || !reg.get<EntityComponent>(proj).isAlive()) {
			reg.get<EntityComponent>(self).kill();
			isDone = true;
			return;
		}
		entities.clear();
		auto& self_body = reg.get<BodyComponent>(self);
		auto& proj_body = reg.get<BodyComponent>(proj);
		self_body.setPosition(proj_body.getPosition());

		if (!reg.valid(target)) {
			target = myecs::null_entity;
		}

		nvec2 shift;
		if (target != myecs::null_entity && hasTarget) {
			auto& target_body = reg.get<BodyComponent>(target);
			shift = target_body.getPosition() - self_body.getPosition();
		}
		else if (!contacts.empty()) {
			auto it = std::max_element(contacts.begin(), contacts.end(), [](const auto& a, const auto& b) {
				return a.lengthSquared < b.lengthSquared;
			});
			shift = it->shift;
			target = it->other;
			contacts.clear();
		}
		else return;

		auto dir = proj_body.getVelocity().normalized();
		nmat22 rot = { dir.y,-dir.x,dir.x,dir.y };
		nvec2 newShift = rot * shift;
		if (std::abs(newShift.x) < nmath::n_epsilon) {
			return;
		}
		float radius = newShift.lengthSquared() / (2.f * std::abs(newShift.x));
		nvec2 forceDir = nvec2{ dir.y,-dir.x }*(newShift.x > 0.f ? 1.f : -1.f);
		float forceSize = proj_body.getVelocity().lengthSquared() * proj_body.body->GetMass() / radius;
		forceSize = std::min(forceSize, 15.f);
		proj_body.body->ApplyForceToCenter(forceDir * forceSize, true);
		hasTarget = false;
	}

	void onContact(GameCtx& ctx, myecs::entity self, myecs::entity other, b2Contact* contact)override {
		contact->SetEnabled(false);
		if (target != myecs::null_entity) {
			if (other == target) {
				hasTarget = true;
			}
			return;
		}
		if (auto it = entities.find(other); it != entities.end()) {
			return;
		}
		auto& reg = ctx.reg;
		auto& self_body = reg.get<BodyComponent>(self);
		auto& other_body = reg.get<BodyComponent>(other);
		auto shift = other_body.getPosition() - self_body.getPosition();
		contacts.push_back(Contact{
			.other = other,
			.shift = shift,
			.lengthSquared = shift.lengthSquared()
						   });
	}

	void onDeath(GameCtx& ctx, myecs::entity self)override {
		auto& reg = ctx.reg;
		if (auto ec = reg.try_get<EntityComponent>(self)) {
			ec->kill();
		}
	}
};