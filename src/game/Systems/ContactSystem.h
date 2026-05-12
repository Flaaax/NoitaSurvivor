#pragma once
#include "src/game/GameContext.h"

class ContactSystem {
public:
	static void handleContactEvent(GameCtx& ctx);
	static void updateAfterHandleEvent(const GameCtx& ctx,float dt);
};