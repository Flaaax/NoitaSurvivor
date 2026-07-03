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
		std::vector<Shared<ProjectileSpell>> projectiles;
		std::vector<Shared<ModifierSpell>> modifiers;
		std::vector<Shared<ModifierSpell>> shotModifiers;

		void add(const Shared<Spell>& spell);
		void cast(const GameCtx& ctx, vec2 pos, float arg);
	};
} // namespace flx::game