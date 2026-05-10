#pragma once
#pragma warning(disable:5260)
#include <imgui.h>

#include"ProjectileSpell.h"
#include"src/global/EntityManager.h"
#include"src/global/AssetManager.h"


class BulletSpell : public ProjectileSpell {
public:
	BulletSpell() {
		castDelay = 0.05f;
		reloadDelay = 0.03f;
		ID = getID<BulletSpell>();
	}

	myecs::entity createProjectile(GameCtx& ctx, const nvec2& worldPos, const nvec2& dir) override;
};


class ExplosionSpell : public ProjectileSpell {
public:
	ExplosionSpell() {
		ID = getID<ExplosionSpell>();
	}

	myecs::entity createProjectile(GameCtx& ctx, const nvec2& worldPos, const nvec2& dir) override;
};