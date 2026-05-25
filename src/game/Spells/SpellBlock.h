#pragma once
#include "../../utils/Vec2/Vec2.h"
#include <memory>
#include <vector>

class ModifierSpell;
class ProjectileSpell;
class Spell;
struct GameCtx;

class SpellBlock {
public:
	std::vector<std::shared_ptr<ProjectileSpell>> projectiles;
	std::vector<std::shared_ptr<ModifierSpell>> modifiers;
	std::vector<std::shared_ptr<ModifierSpell>> shotModifiers;

	void add(const std::shared_ptr<Spell>& spell);
	void cast(const GameCtx& ctx, nvec2 pos, float arg);
};