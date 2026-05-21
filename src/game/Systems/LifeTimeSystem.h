#pragma once

struct GameCtx;

class LifeTimeSystem {
public:
	static void cleanupDeadEntities(const GameCtx& ctx);
};