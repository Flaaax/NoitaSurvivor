#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Pointer.h"

#include <vector>

namespace flx::game {
	class ModifierSpell;
	class ProjectileSpell;
	class Spell;
	struct GameCtx;

	class SpellBlock {
	public:
		std::vector<n_shared<ProjectileSpell>> projectiles;
		std::vector<n_shared<ModifierSpell>> modifiers;
		std::vector<n_shared<ModifierSpell>> shotModifiers;

		void add(n_shared<Spell> spell);
		void cast(const GameCtx& ctx, vec2 pos, float arg);
	};
} // namespace flx::game