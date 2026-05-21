#include "Modifiers.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/Script/Scripts.h"
#include "src/game/Game.h"
#include "src/game/Services/ContactService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Spells/Projectiles/Projectiles.h"
#include "src/game/Spells/SpellBlock.h"
#include "src/utils/VectorHelper.h"
#include <memory>
#include <src/game/Components/PhysicsComponents.h>
#include <src/game/Components/Render/RenderComponent.h>
#include <src/game/Spells/Shot.h>

void AddExplosionSpell::apply(const GameCtx& ctx, myecs::entity p) {
	auto& reg = ctx.reg;
	auto& pc = reg.get<ProjectileComponent>(p);
	reg.get_or_emplace<SpellOnDeathComponent>(p).spellBlock.add(std::make_shared<ExplosionSpell>());
}

void HomingShot::apply(const GameCtx& ctx, myecs::entity p) {
	ctx.reg.get_or_emplace<ScriptComponent>(p).scripts += Util::makeShared(new TrackingScript(radius));
	// const auto detector = reg.create();
	// reg.emplace<BodyComponent>(detector);
	// reg.emplace<EntityComponent>(detector).layer = PlayerProjectile;
	//
	// const BodyArg arg{
	// 	.type = BodyArg::Static,
	// 	.fixedRotation = true,
	// 	.shape = BodyArg::Circle,
	// 	.radius = radius,
	// };
	//
	// PhysicsService().createBody(ctx, detector, arg);
	//
	// reg.get_or_emplace<ScriptComponent>(detector).scripts += Util::makeShared(new CircleTrackingDetectorScript(ctx, detector, p));
}

MultiShots::MultiShots(int shots) {
	if (shots >= 2) {
		this->shots = shots;
	} else {
		Logger::warn("MutiShots doesnt not accept shots: {}, fallback to default: 3", shots);
	}
	castDelay = 0.1f;
	reloadDelay = 0.1f;
	tags.add(Tag::SHOT_MODIFY);

	ID = getID<MultiShots>();
}

void MultiShots::modifyShot(std::vector<ShotData>& data) {
	std::vector<ShotData> ret;
	for (const auto& dat : data) {
		const float scatter = Util::to_rad(Util::min(static_cast<float>(shots * 15 + 15), 180.F));
		const float scatter_step = scatter / static_cast<float>(shots - 1);
		float cur_scatter = scatter / 2.f + dat.arg;
		for (int i = 0; i < shots; i++) {
			ret.push_back({.pos = dat.pos,
						   .arg = cur_scatter});
			cur_scatter -= scatter_step;
		}
	}
	ret.swap(data);
}

/*
class TrackingDetectorScript :public Script {
public:
	myecs::entity proj;
	std::vector<std::pair<nvec2, float>> contacts;

	TrackingDetectorScript(myecs::entity proj) {
		this->proj = proj;
	}

	void onUpdate(myecs::entity self, float dt)override {
		auto& reg = Game::getRegistry();
		if (!reg.valid(proj) || !reg.get<EntityComponent>(proj).isAlive()) {
			reg.get<EntityComponent>(self).kill();
			isDone = true;
			return;
		}
		auto& self_body = reg.get<BodyComponent>(self);
		auto& proj_body = reg.get<BodyComponent>(proj);
		self_body.setPosition(*proj_body.position);

		auto& s = reg.get<SmoothMovementComponent>(proj);
		if (contacts.empty()) {
			s.dir = proj_body.velocity->normalized();
			return;
		}
		auto it = std::max_element(contacts.begin(), contacts.end(), [](const auto& a, const auto& b) {
			return a.second < b.second;
		});
		if (it->second < nmath::n_epsilon) {
			s.dir = {};
		}
		else {
			s.dir = it->first / std::sqrt(it->second);
		}
		contacts.clear();
		reg.get<MultipleContactComponent>(self).disabledContacts.clear();
	}

	void onContact(myecs::entity self, myecs::entity other, b2Contact* contact)override {
		contact->SetEnabled(false);
		auto& reg = Game::getRegistry();
		auto& self_body = reg.get<BodyComponent>(self);
		auto& other_body = reg.get<BodyComponent>(other);
		auto shift = *other_body.position - *self_body.position;
		contacts.push_back({ shift, shift.lengthSquared() });
	}

	void onDeath(myecs::entity self)override {
		auto& reg = Game::getRegistry();
		if (auto ec = reg.try_get<EntityComponent>(self)) {
			ec->kill();
		}
	}
};
*/

void Parasite::apply(const GameCtx& ctx, myecs::entity p) {
	class ParasiteScript : public Script {
	private:
		Parasite* this_spell;

	public:
		explicit ParasiteScript(Parasite* this_spell) : this_spell(this_spell) {
		}

		void onDeath(const GameCtx& ctx, myecs::entity self) override {
			auto& reg = ctx.reg;
			auto& p = reg.get<ProjectileComponent>(self);
			const auto& b = reg.get<BodyComponent>(self);
			PhysicsService ps{};
			const auto dir = ps.getVelocity(b).normalized();
			auto pos = ps.getPosition(b);
			for (auto& mod : p.mods) {
				if (mod.get() == this_spell) {
					mod = {};
					break;
				}
			}
			for (auto left = nmath::ROT_LEFT * dir; auto& new_dir : {left, -left}) {
				const auto proj1 = Shot::clone(ctx, self, pos, Util::to_rad(new_dir));
				auto& p1 = reg.get_or_emplace<MultiContactComponent>(proj1);
				p1.banned.merge(reg.get<MultiContactComponent>(self).banned);
			}
			// todo damage modifier
			// std::cout << "triggered!\n";
		}
	};

	if (!script) {
		script = std::make_shared<ParasiteScript>(this);
	}

	auto& reg = ctx.reg;
	reg.get_or_emplace<MultiContactComponent>(p);
	reg.get_or_emplace<ScriptComponent>(p).scripts += script;
}