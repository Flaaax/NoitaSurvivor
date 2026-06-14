#pragma once

namespace flx::game {
	struct GameCtx;

	class LifeTimeSystem {
	public:
		static void cleanupDeadEntities(const GameCtx& ctx);
	};
} // namespace flx::game