#pragma once

namespace flx::game {
	struct GameCtx;

	class PlayerService {
	public:
		static void gainMaterial(const GameCtx& ctx, int value);
	};
} // namespace flx::game