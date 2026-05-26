#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "Spell.h"

// factory class
class ProjectileSpell : public Spell {
public:
	explicit ProjectileSpell(std::string_view ID) : Spell(ID, baseKind){}

	N_CONSTEXPR Kind baseKind = Spell::ProjectileSpell;

	virtual myecs::entity createProjectile(const GameCtx& ctx, nvec2 worldPos, nvec2 dir) = 0;
};
