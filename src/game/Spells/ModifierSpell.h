#pragma once
#include "Spell.h"

struct ShotData {
	nvec2 pos = {};
	float arg = {};
};

class ModifierSpell : public Spell {
public:
	explicit ModifierSpell(std::string_view ID) : Spell(ID, baseKind) {}

	N_CONSTEXPR Kind baseKind = Spell::ModifierSpell;

	virtual void apply(const GameCtx& ctx, myecs::entity p) {}

	// Do anything to the original data
	virtual void modifyShot(std::vector<ShotData>& data) {
		//throw NMethodNotImplementedException();
	}
};
