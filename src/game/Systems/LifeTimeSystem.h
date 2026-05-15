#pragma once

struct GameCtx;

class LifeTimeSystem {
public:
	static void destroyDeadEntities(const GameCtx& ctx);

};