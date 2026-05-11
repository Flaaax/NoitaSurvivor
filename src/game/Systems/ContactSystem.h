#pragma once
#include "src/game/GameContext.h"

class ContactSystem {
public:
	static void update(GameCtx& ctx, float dt);
	static void clearEvent(GameCtx& ctx);
};