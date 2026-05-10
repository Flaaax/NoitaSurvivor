#pragma once

struct GameCtx;

class EntityDestroySystem {
public:
	void destroyDeadEntities(GameCtx& ctx);

};