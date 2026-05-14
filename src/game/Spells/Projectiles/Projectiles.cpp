#include "Projectiles.h"
#include "src/game/Components/EntityFactory.h"

myecs::entity BulletSpell::createProjectile(const GameCtx& ctx, nvec2 worldPos, nvec2 dir) {
	/*auto bullet = EntityManager::instance().getEntity<Bullet>();
		if (!bullet) {
			MYASSERT(true, "invalid entity");
			return bullet;
		}
		bullet->setData(worldPos, 40.f * dir);
		bullet->scattering = Util::to_rad(10.f);
		return bullet;*/

	return ctx.factory.createBullet(ctx, worldPos, 30.f * dir);
}

myecs::entity ExplosionSpell::createProjectile(const GameCtx& ctx, nvec2 worldPos, nvec2 dir) {
	return ctx.factory.createExplosion(ctx, worldPos, 2.5, 30);
}
