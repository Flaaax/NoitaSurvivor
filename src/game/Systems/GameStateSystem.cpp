#include "GameStateSystem.h"
#include "../../ui/global/NInput.h"
#include "../GameContext.h"
#include "src/game/Components/EntityFactory.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Wands/Wand.h"
#include "src/utils/Pointer.h"
#include "src/utils/Random.h"
#include "src/utils/VectorHelper.h"

void GameStateSystem::initGameState(const GameCtx& ctx) {
	using namespace Util;

	auto& state = ctx.gameState;

	state.wands += makeUnique(new Wand());

	state.player.id = ctx.factory.createPlayer(ctx);
	state.player.collector = ctx.factory.createCollector(ctx, 4.5f);
	state.player.maxExp = 20;

	ctx.factory.createMaterial(ctx, {6.f, 6.f});

	nrect debugEnemySpawnArea = state.debugEnemySpawnArea;

	// test
	state.enemySpawnTimer.set(1.0f, [ctx, debugEnemySpawnArea] {
							 const nvec2 spawnPos{
								 random.nextFloat(debugEnemySpawnArea.left(), debugEnemySpawnArea.right()),
								 random.nextFloat(debugEnemySpawnArea.top(), debugEnemySpawnArea.bottom()),
							 };
							 ctx.factory.createEnemy(ctx, spawnPos);
						 })
		.start(-1);

	const nrect bound = state.bound;

	std::vector<std::pair<nvec2, nvec2>> borderEdges = {
		{bound.leftTop(), bound.rightTop()},
		{bound.rightTop(), bound.rightBottom()},
		{bound.rightBottom(), bound.leftBottom()},
		{bound.leftBottom(), bound.leftTop()}};
	for (auto& [fst, snd] : borderEdges) {
		state.borders += ctx.factory.createBorder(ctx, fst, snd);
	}
}

void GameStateSystem::updateBeforePhysics(const GameCtx& ctx) {
	auto& state = ctx.gameState;
	auto& inputState = NInput::getState();
	state.mousePos = (inputState.mouseRender - NWindow::scale.gameRenderOffset) / NWindow::scale.gameRenderScale + state.cameraPos;
	const myecs::entity player = ctx.gameState.player.id;
	state.playerPos = PhysicsService().getPosition(ctx, player);
	state.cameraPos = state.playerPos;
}
