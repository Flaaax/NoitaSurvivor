#pragma once
#include "src/ecs/entity.h"
#include "src/game/GameContext.h"

namespace flx::game {
	class Game;

	class PhysicsSystem {
	public:
		static void step(const GameCtx& ctx, float dt);
		static void updateAfterContactSystem(const GameCtx& ctx, float dt);
	};
} // namespace flx::game