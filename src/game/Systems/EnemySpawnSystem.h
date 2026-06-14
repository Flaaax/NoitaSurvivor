#pragma once
#include "src/utils/Vec2/Vec2.h"

#include <string_view>

namespace flx::game {
	struct GameCtx;

	class EnemySpawnSystem {
	private:
		static void debugEnemeyBirth(const GameCtx& ctx, vec2 pos, std::string_view name);

	public:
		static void setup(const GameCtx& ctx);
		// static void update(const GameCtx& ctx, float dt);
		static void updateAfterCleanup(const GameCtx& ctx, float dt);
	};
} // namespace flx::game