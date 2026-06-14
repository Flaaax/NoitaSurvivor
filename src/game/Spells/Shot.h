#pragma once
#include "ModifierSpell.h"
#include "ProjectileSpell.h"
//"src/global/hooks/ProjectileHook.h"
// #include"src/events/EventManager.h"
// #include"src/events/args/AddProjectileEvent.h"
#include "src/utils/Pointer.h"
#include <src/game/GameContext.h>

namespace flx::game {
	class Shot {
	private:
		static void modify(const GameCtx& ctx, myecs::entity proj, const n_shared<ProjectileSpell>& ps, const std::vector<n_shared<ModifierSpell>>& projMods);

	public:
		static myecs::entity fire(const GameCtx& ctx, const n_shared<ProjectileSpell>& ps, const std::vector<n_shared<ModifierSpell>>& projMods, vec2 shotPos, float arg);
		// The clone spell itself should be excluded manually in the script
		static myecs::entity clone(const GameCtx& ctx, myecs::entity proj, vec2 shotPos, float arg);
	};
} // namespace flx::game