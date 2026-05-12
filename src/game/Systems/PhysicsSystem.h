#pragma once
#include"src/ecs/entity.h"
#include"src/game/GameContext.h"

class Renderer;
class Game;
struct nvec2;

class PhysicsSystem {
public:
	static void step(const GameCtx& ctx, float dt);
	static void updateAfterContactSystem(const GameCtx& ctx, float dt);
};