#pragma once
// #pragma warning(disable : 5260)

#include "../ProjectileSpell.h"

namespace flx::game {
	DEF_SPELL(BulletSpell, ProjectileSpell) {
	public:
		BulletSpell() {
			castDelay = 0.05f;
			reloadDelay = 0.03f;
		}

		myecs::entity createProjectile(const GameCtx& ctx, vec2 worldPos, vec2 dir) override;
	};

	DEF_SPELL(ExplosionSpell, ProjectileSpell) {
	public:
		ExplosionSpell() {}

		myecs::entity createProjectile(const GameCtx& ctx, vec2 worldPos, vec2 dir) override;
	};
} // namespace flx::game