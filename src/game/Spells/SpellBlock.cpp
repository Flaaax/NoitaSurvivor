#include "SpellBlock.h"
#include "Modifiers/ModifierSpell.h"
#include "Projectiles/ProjectileSpell.h"
#include "Shot.h"


void SpellBlock::add(const std::shared_ptr<Spell>& spell) {
	if (spell->getSpellType() == Spell::MODIFIER_SPELL) {
		auto mod = std::static_pointer_cast<ModifierSpell>(spell);
		if (mod->tags.has(Spell::Tag::SHOT_MODIFY)) {
			shotModifiers.emplace_back(mod);
		}
		modifiers.emplace_back(std::move(mod));
	}
	else if (spell->getSpellType() == Spell::PROJECTILE_SPELL) {
		projectiles.emplace_back(std::static_pointer_cast<ProjectileSpell>(spell));
	}
	else throw std::runtime_error("invalid spell!");
}

void SpellBlock::cast(GameCtx& ctx, nvec2 pos, float arg) {
	std::vector<ShotData> shots = { {.pos = pos,.arg = arg} };
	for (auto& mod : shotModifiers) {
		mod->modifyShot(shots);
	}
	for (auto& projSpell : projectiles) {
		for (auto& shot : shots) {
			Shot::fire(ctx, projSpell, modifiers, shot.pos, shot.arg);
		}
	}
}