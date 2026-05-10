#pragma once
#include<box2d/b2_world_callbacks.h>
#include"src/game/GameContext.h"

class GameContactFilter :public b2ContactFilter {
private:
	GameCtx ctx;
public:
	GameContactFilter(GameCtx ctx) :ctx(ctx) {}

	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override;
};