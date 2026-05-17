#include "GameStateSystem.h"
#include "../GameContext.h"
#include "src/game/Components/EntityFactory.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Wands/Wand.h"
#include "src/global/InputManager.h"
#include "src/utils/Pointer.h"
#include "src/utils/VectorHelper.h"

void GameStateSystem::initGameState(const GameCtx& ctx) {
	using namespace Util;

	auto& state = ctx.gameState;

	state.wands += makeUnique(new Wand());

	state.player.id = ctx.factory.createPlayer(ctx);
	state.player.collector = ctx.factory.createCollector(ctx, 3.f);
	state.player.maxExp = 20;

	ctx.factory.createMaterial(ctx, {6.f, 6.f});

	// test
	state.enemySpawnTimer.set(1.0f, [ctx] {
							 ctx.factory.createEnemy(ctx, {0, 0});
						 })
		.start(-1);

	const nrect bound = state.bound;

	std::vector<std::pair<nvec2, nvec2>> borderEdges = {
		{bound.topLeft(), bound.topRight()},
		{bound.topRight(), bound.bottomRight()},
		{bound.bottomRight(), bound.bottomLeft()},
		{bound.bottomLeft(), bound.topLeft()}};
	for (auto& [fst, snd] : borderEdges) {
		state.borders += ctx.factory.createBorder(ctx, fst, snd);
	}
}

void GameStateSystem::updateBeforePhysics(const GameCtx& ctx) {
	auto& state = ctx.gameState;
	auto& inputState = InputManager::getState();
	state.mousePos = (inputState.mouseRender - NWindow::scale.gameRenderOffset) / NWindow::scale.gameRenderScale + state.cameraPos;
	const myecs::entity player = ctx.gameState.player.id;
	state.playerPos = PhysicsService().getPosition(ctx, player);
	state.cameraPos = state.playerPos;
}
