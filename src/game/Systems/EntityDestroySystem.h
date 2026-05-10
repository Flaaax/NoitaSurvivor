#pragma once

struct GameCtx;

class EntityDestroySystem {
public:
	static void destroyDeadEntities(GameCtx& ctx);

};