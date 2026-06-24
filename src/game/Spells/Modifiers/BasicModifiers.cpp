#include "BasicModifiers.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/Script/Scripts.h"
#include "src/game/Game.h"
#include "src/game/Services/ContactService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Spells/Projectiles/BasicProjectiles.h"
#include "src/game/Spells/SpellBlock.h"
#include "src/utils/VectorHelper.h"
#include <memory>
#include <src/game/Components/PhysicsComponents.h>
#include <src/game/Components/Render/RenderComponent.h>
#include <src/game/Spells/Shot.h>

namespace flx::game {
	void AddExplosionSpell::apply(const GameCtx& ctx, myecs::entity p) {
		auto& reg = ctx.reg;
		// auto& pc = reg.get<ProjectileComponent>(p);
		reg.get_or_emplace<SpellOnDeathComponent>(p).spellBlock.add(std::make_shared<ExplosionSpell>());
	}

	void HomingShot::apply(const GameCtx& ctx, myecs::entity p) {
		ctx.reg.get_or_emplace<ScriptComponent>(p).scripts += flx::makeShared(new TrackingScript(radius));
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
			logger.warn("MutiShots doesnt not accept shots: {}, fallback to default: 3", shots);
		}
		castDelay = 0.1f;
		reloadDelay = 0.1f;
		tags.add(Tag::SHOT_MODIFY);
	}

	void MultiShots::modifyShot(std::vector<ShotData>& data) {
		std::vector<ShotData> ret;
		for (const auto& dat : data) {
			const float scatter = math::to_rad(std::min(static_cast<float>(shots * 15 + 15), 180.F));
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

	ScatterShot::ScatterShot() {
		castDelay = -0.3f;
		reloadDelay = -0.5f;
		scatter = 3.f * math::pi;
	}

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
				for (auto left = mat22::rotLeft() * dir; auto& new_dir : {left, -left}) {
					const auto proj1 = Shot::clone(ctx, self, pos, new_dir.rad());
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
} // namespace flx::game