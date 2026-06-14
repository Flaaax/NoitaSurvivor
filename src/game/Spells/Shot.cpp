#include "Shot.h"

#include "src/utils/Math.h"

#include <src/game/Components/EntityComponents.h>
#include <src/utils/Random.h>

namespace flx::game {
	void Shot::modify(const GameCtx& ctx, myecs::entity proj, const n_shared<ProjectileSpell>& ps, const std::vector<n_shared<ModifierSpell>>& projMods) {
		auto& p = ctx.reg.get<ProjectileComponent>(proj);
		p.spell = ps;
		p.mods = projMods;
		for (auto& mod : projMods) {
			if (mod) {
				mod->apply(ctx, proj);
			}
		}
	}

	myecs::entity Shot::fire(const GameCtx& ctx, const n_shared<ProjectileSpell>& ps, const std::vector<n_shared<ModifierSpell>>& projMods, vec2 shotPos, float arg) {
		float proj_scatter = ps->scatter;
		for (const auto& mod : projMods) {
			if (!mod)
				continue;
			proj_scatter += mod->scatter;
		}
		const float rand_scatter = flx::randomScatter(proj_scatter);
		const auto p = ps->createProjectile(ctx, shotPos, math::from_rad(arg + rand_scatter));
		modify(ctx, p, ps, projMods);
		return p;
	}

	myecs::entity Shot::clone(const GameCtx& ctx, myecs::entity proj, vec2 shotPos, float arg) {
		auto& s = ctx.reg.get<ProjectileComponent>(proj);
		return fire(ctx, s.spell, s.mods, shotPos, arg);
	}
} // namespace flx::game