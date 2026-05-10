#include"Shot.h"
#include <src/game/Components/EntityComponents.h>
#include <src/utils/Random.h>


void Shot::modify(GameCtx& ctx, myecs::entity proj, n_shared<ProjectileSpell>& ps, std::vector<n_shared<ModifierSpell>>& projMods) {
	auto& p = ctx.reg.get<ProjectileComponent>(proj);
	p.spell = ps;
	p.mods = projMods;
	for (auto& mod : projMods) {
		if (mod) {
			mod->apply(ctx, proj);
		}
	}
}

myecs::entity Shot::fire(GameCtx& ctx, n_shared<ProjectileSpell>& ps, std::vector<n_shared<ModifierSpell>>& projMods, const nvec2& shotPos, float arg) {
	float proj_scatter = ps->scatter;
	for (const auto& mod : projMods) {
		if (!mod)continue;
		proj_scatter += mod->scatter;
	}
	float rand_scatter = Util::randomScatter(proj_scatter);
	auto p = ps->createProjectile(ctx, shotPos, Util::from_rad<nvec2>(arg + rand_scatter));
	modify(ctx, p, ps, projMods);
	return p;
}

myecs::entity Shot::clone(GameCtx& ctx, myecs::entity proj, nvec2 shotPos, float arg) {
	auto& s = ctx.reg.get<ProjectileComponent>(proj);
	return fire(ctx, s.spell, s.mods, shotPos, arg);
}
