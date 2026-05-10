#pragma once
#include"../Spells/Projectiles/ProjectileSpell.h"
#include"Modifiers/ModifierSpell.h"
//"src/global/hooks/ProjectileHook.h"
//#include"src/events/EventManager.h"
//#include"src/events/args/AddProjectileEvent.h"
#include"src/utils/Pointer.h"
#include <src/game/GameContext.h>

class Shot {
private:
	static void modify(GameCtx& ctx, myecs::entity proj, n_shared<ProjectileSpell>& ps, std::vector<n_shared<ModifierSpell>>& projMods);

public:
	static myecs::entity fire(GameCtx& ctx, n_shared<ProjectileSpell>& ps, std::vector<n_shared<ModifierSpell>>& projMods, const nvec2& shotPos, float arg);
	//The clone spell itself should be excluded manually in the script
	static myecs::entity clone(GameCtx& ctx, myecs::entity proj, nvec2 shotPos, float arg);
};

