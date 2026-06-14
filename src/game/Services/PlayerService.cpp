#include "PlayerService.h"

#include "src/game/GameContext.h"

namespace flx::game {
	void PlayerService::gainMaterial(const GameCtx& ctx, int value) {
		auto& state = ctx.gameState;
		state.player.material += value;
		state.player.exp += value;
		while (state.player.exp >= state.player.maxExp) {
			state.player.exp -= state.player.maxExp;
			state.player.level++;
			state.player.maxExp += 5;
		}
	}
} // namespace flx::game