#include"Projectiles.h"
#include"src/game/Components/EntityFactory.h"

myecs::entity BulletSpell::createProjectile(GameCtx& ctx, const nvec2& worldPos, const nvec2& dir) {
	/*auto bullet = EntityManager::instance().getEntity<Bullet>();
		if (!bullet) {
			MYASSERT(true, "invalid entity");
			return bullet;
		}
		bullet->setData(worldPos, 40.f * dir);
		bullet->scattering = Util::to_rad(10.f);
		return bullet;*/

	return ctx.factory.createBullet(worldPos, 30.f * dir);
}

myecs::entity ExplosionSpell::createProjectile(GameCtx& ctx, const nvec2& worldPos, const nvec2& dir) {
	return ctx.factory.createExplosion(worldPos, 2.5, 30);
}
