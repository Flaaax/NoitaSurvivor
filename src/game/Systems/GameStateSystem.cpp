#include "GameStateSystem.h"
#include "../GameContext.h"
#include "src/app/Application.h"
#include "src/game/Components/EntityFactory.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Wands/Wand.h"
#include "src/game/render/RenderScales.h"
#include "src/utils/Pointer.h"
#include "src/utils/Random.h"

namespace flx::game {
	void GameStateSystem::setup(const GameCtx& ctx) {
		using namespace flx;

		auto& state = ctx.gameState;

		state.wandManager.addWand(ctx.scales.wandScale);
		state.wandManager.addWand(ctx.scales.wandScale);
		state.wandManager.addWand(ctx.scales.wandScale);
		state.wandManager.addWand(ctx.scales.wandScale);

		state.player.id = ctx.factory.createPlayer(ctx);
		state.player.collector = ctx.factory.createCollector(ctx, 4.5f);
		state.player.maxExp = 20;

		ctx.factory.createMaterial(ctx, {6.f, 6.f});

		state.bound = rect::fromCenter({}, {55, 55});
		state.maxBound = rect::fromCenter({}, {100, 100});

		state.debugEnemySpawnArea = rect::fromCenter({0, 0}, {35, 35});

		const rect bound = state.bound;

		Vector<Pair<vec2>> borderEdges = {
			{bound.leftTop(), bound.rightTop()},
			{bound.rightTop(), bound.rightBottom()},
			{bound.rightBottom(), bound.leftBottom()},
			{bound.leftBottom(), bound.leftTop()},
		};
		for (auto& [fst, snd] : borderEdges) {
			state.borders += ctx.factory.createBorder(ctx, fst, snd);
		}
	}

	void GameStateSystem::updateBeforePhysics(const GameCtx& ctx) {
		auto& state = ctx.gameState;
		auto& inputState = ctx.appCtx.input;
		state.mousePos = (inputState.mouseRender - ctx.scales.offset) / ctx.scales.scale + state.camera.position;
		const auto player = ctx.gameState.player.id;
		state.playerPos = PhysicsService().getPosition(ctx, player);
		state.playerVelocity = PhysicsService().getVelocity(ctx, player);
		state.camera.position = state.playerPos;
	}

	void GameStateSystem::updateAfterPhysics(const GameCtx& ctx) {
		auto& state = ctx.gameState;
		const auto player = ctx.gameState.player.id;
		state.playerPos = PhysicsService().getPosition(ctx, player);
		state.playerVelocity = PhysicsService().getVelocity(ctx, player);
		// state.cameraPos = state.playerPos;
	}
} // namespace flx::game