#pragma once
#pragma warning(disable : 5260)

#include "../ProjectileSpell.h"

DEF_SPELL(BulletSpell, ProjectileSpell) {
public:
	BulletSpell() {
		castDelay = 0.05f;
		reloadDelay = 0.03f;
	}

	myecs::entity createProjectile(const GameCtx& ctx, nvec2 worldPos, nvec2 dir) override;
};

DEF_SPELL(ExplosionSpell, ProjectileSpell) {
public:
	ExplosionSpell() {}

	myecs::entity createProjectile(const GameCtx& ctx, nvec2 worldPos, nvec2 dir) override;
};