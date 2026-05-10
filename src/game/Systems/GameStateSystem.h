#pragma once

struct GameCtx;

class GameStateSystem {
public:
	void initStates(GameCtx& ctx);
	void updateBeforePhysics(GameCtx& ctx);
};