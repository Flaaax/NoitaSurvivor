#include "PlayerService.h"

void PlayerService::gainMaterial(GameCtx& ctx, int value) {
	auto& state = ctx.state;
	state.player.material += value;
	state.player.exp += value;
	while (state.player.exp >= state.player.maxExp) {
		state.player.exp -= state.player.maxExp;
		state.player.level++;
		state.player.maxExp += 5;
	}
}
