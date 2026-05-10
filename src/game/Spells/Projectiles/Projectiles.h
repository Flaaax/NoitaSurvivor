#pragma once
#pragma warning(disable:5260)
#include"ProjectileSpell.h"
#include"src/global/EntityManager.h"
#include"src/global/AssetManager.h"


class BulletSpell :public ProjectileSpell {
public:
	BulletSpell() {
		castDelay = 0.05f;
		reloadDelay = 0.03f;
		textureName = "bullet";
		//scatter = 60;
	}

	myecs::entity createProjectile(GameCtx& ctx, const nvec2& worldPos, const nvec2& dir) override;
};


class ExplosionSpell :public ProjectileSpell {
public:
	myecs::entity createProjectile(GameCtx& ctx, const nvec2& worldPos, const nvec2& dir) override;
};