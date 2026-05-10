#pragma once

struct GameCtx;

class GameStateSystem {
public:
	static void initStates(GameCtx& ctx);

	static void updateBeforePhysics(GameCtx& ctx);
};