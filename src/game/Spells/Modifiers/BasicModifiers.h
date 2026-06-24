#pragma once
#include "../ModifierSpell.h"
#include "src/utils/Math.h"

namespace flx::game {
	DEF_SPELL(AddExplosionSpell, ModifierSpell) {
	public:
		AddExplosionSpell() {
			castDelay = 0.2f;
			reloadDelay = 0.3f;
		}

		void apply(const GameCtx& ctx, myecs::entity p) override;
	};

	DEF_SPELL(MultiShots, ModifierSpell) {
	private:
		int shots = 2;

	public:
		explicit MultiShots(int shots = 2);

		void modifyShot(std::vector<ShotData> & data) override;
	};

	DEF_SPELL(ScatterShot, ModifierSpell) {
	public:
		ScatterShot();
	};

	DEF_SPELL(HomingShot, ModifierSpell) {
	public:
		float radius;

		explicit HomingShot(float radius = 4.f) : radius(radius) {
			castDelay += 0.2f;
			reloadDelay += 0.4f;
		}

		void apply(const GameCtx& ctx, myecs::entity p) override;
	};

	class Script;

	DEF_SPELL(Parasite, ModifierSpell) {
	private:
		Shared<Script> script;

	public:
		Parasite() {
			delayMultiplier = 2.f;
		}

		void apply(const GameCtx& ctx, myecs::entity p) override;
	};
} // namespace flx::game