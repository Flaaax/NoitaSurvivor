#pragma once

namespace flx::game {
	struct GameCtx;

	class GameStateSystem {
	public:
		static void initGameState(const GameCtx& ctx);

		static void updateBeforePhysics(const GameCtx& ctx);
	};
} // namespace flx::game