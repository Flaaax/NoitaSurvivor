#pragma once
#include "src/ecs/entity.h"
#include "src/game/Contact/ContactLayerRules.h"
#include "src/game/GameContext.h"

class EntityService {
public:
	static void damage(const GameCtx& ctx, myecs::entity source, myecs::entity target, int damage);
	static void kill(const GameCtx& ctx, myecs::entity e);
	static void killSilent(const GameCtx& ctx, myecs::entity e);	//for debug or cleanup usage
	static bool isAlive(const GameCtx& ctx, myecs::entity e);
	static bool isValidAndAlive(const GameCtx& ctx, myecs::entity e);
	static ContactLayer getLayer(const GameCtx& ctx, myecs::entity e);
	static void clearMostEntities(const GameCtx& ctx);	//for debug or cleanup usage
};