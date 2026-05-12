#pragma once
#include "src/ecs/entity.h"
#include "src/game/GameContext.h"

class EntityService {
public:
	static void damage(const GameCtx& ctx, myecs::entity source, myecs::entity target, int damage);
	static void kill(const GameCtx& ctx, myecs::entity e);
	static bool isAlive(const GameCtx& ctx, myecs::entity e);
};