#include "SpellBlock.h"
#include "ModifierSpell.h"
#include "ProjectileSpell.h"
#include "Shot.h"

namespace flx::game {
	void SpellBlock::add(const Shared<Spell>& spell) {
		if (spell->getKind() == Spell::ModifierSpell) {
			auto mod = std::static_pointer_cast<ModifierSpell>(spell);
			if (mod->tags.has(Spell::Tag::SHOT_MODIFY)) {
				shotModifiers.emplace_back(mod | move);
			}
			modifiers.emplace_back(std::move(mod));
		} else if (spell->getKind() == Spell::ProjectileSpell) {
			projectiles.emplace_back(std::static_pointer_cast<ProjectileSpell>(spell));
		} else
			throw std::runtime_error("invalid spell!");
	}

	void SpellBlock::cast(const GameCtx& ctx, vec2 pos, float arg) {
		std::vector<ShotData> shots = {{.pos = pos, .arg = arg}};
		for (auto& mod : shotModifiers) {
			mod->modifyShot(shots);
		}
		for (auto& projSpell : projectiles) {
			for (auto& shot : shots) {
				Shot::fire(ctx, projSpell, modifiers, shot.pos, shot.arg);
			}
		}
	}
} // namespace flx::game