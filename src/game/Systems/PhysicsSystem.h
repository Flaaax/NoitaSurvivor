#pragma once
#include"src/ecs/entity.h"
#include"src/game/GameContext.h"

class Renderer;
class Game;
struct nvec2;

class PhysicsSystem {
public:
	static void step(float dt,GameCtx& ctx);
	static void update(float dt, GameCtx& ctx);
};