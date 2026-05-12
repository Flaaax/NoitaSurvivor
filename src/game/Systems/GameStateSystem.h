#pragma once

struct GameCtx;

class GameStateSystem {
public:
	static void initGameState(GameCtx& ctx);

	static void updateBeforePhysics(GameCtx& ctx);
};