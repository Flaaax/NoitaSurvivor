#pragma once
#include"../Spell.h"
#include"src/ecs/entity.h"
#include"src/utils/Vec2.h"


//factory class
class ProjectileSpell : public Spell {
protected:
public:
    int projectile_count = 1;
    float scatter = 0.f;
    float lifeTime = -1.f;

    ProjectileSpell() {
        drawModifier = -1;
    }

    SpellType getSpellType() const override { return Spell::PROJECTILE_SPELL; }

    virtual myecs::entity createProjectile(const GameCtx& ctx, const nvec2& worldPos, const nvec2& dir) = 0;
};
