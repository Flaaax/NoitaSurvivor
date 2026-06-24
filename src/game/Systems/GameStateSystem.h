#pragma once

namespace flx::game {
	struct GameCtx;

	class GameStateSystem {
	public:
		static void setup(const GameCtx& ctx);
		static void updateBeforePhysics(const GameCtx& ctx);
		static void updateAfterPhysics(const GameCtx& ctx);
	};
} // namespace flx::game