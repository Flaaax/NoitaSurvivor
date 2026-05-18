#pragma once
#pragma warning(disable:5260)

#include"ProjectileSpell.h"


class BulletSpell : public ProjectileSpell {
public:
	BulletSpell() {
		castDelay = 0.05f;
		reloadDelay = 0.03f;
		ID = getID<BulletSpell>();
	}

	myecs::entity createProjectile(const GameCtx& ctx, nvec2 worldPos, nvec2 dir) override;
};


class ExplosionSpell : public ProjectileSpell {
public:
	ExplosionSpell() {
		ID = getID<ExplosionSpell>();
	}

	myecs::entity createProjectile(const GameCtx& ctx, nvec2 worldPos, nvec2 dir) override;
};