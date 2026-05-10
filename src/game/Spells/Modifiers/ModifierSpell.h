#pragma once
#include"../Spell.h"
#include"../Projectiles/ProjectileSpell.h"
#include"src/ecs/entity.h"
#include"src/utils/Exception.h"


struct ShotData {
	nvec2 pos = {};
	float arg = {};
};

class ModifierSpell :public Spell {
public:
	float dmg_mod = 0;
	float velocity_mod = 0;
	float acceleration_mod = 0;
	float scatter = 0.f;

	SpellType getSpellType()const override { return Spell::MODIFIER_SPELL; }

	virtual void apply(GameCtx& ctx, myecs::entity p) {}

	//Do anything to the original data
	virtual void modifyShot(std::vector<ShotData>& data) {
		throw NMethodNotImplementedException();
	}
};
