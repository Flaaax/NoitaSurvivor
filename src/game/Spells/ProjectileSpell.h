#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "Spell.h"

namespace flx::game {
	// factory class
	class ProjectileSpell : public Spell {
	public:
		explicit ProjectileSpell(std::string_view ID) : Spell(ID, baseKind) {}

		FLX_CONSTEXPR Kind baseKind = Spell::ProjectileSpell;

		virtual myecs::entity createProjectile(const GameCtx& ctx, vec2 worldPos, vec2 dir) = 0;
	};
} // namespace flx::game