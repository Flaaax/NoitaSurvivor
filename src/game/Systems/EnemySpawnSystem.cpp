#include "EnemySpawnSystem.h"

#include "src/app/global/DebugVariables.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/EntityFactory.h"
#include "src/game/GameContext.h"
#include "src/game/Services/EntityService.h"
#include "src/utils/Random.h"

namespace flx::game {
	void EnemySpawnSystem::debugEnemeyBirth(const GameCtx& ctx, vec2 pos, std::string_view name) {
		ctx.factory.createEnemyBirth(ctx, pos, name);
	}

	void EnemySpawnSystem::setup(const GameCtx& ctx) {
		rect debugEnemySpawnArea = ctx.gameState.debugEnemySpawnArea;

		// test
		ctx
			.gameState
			.enemySpawnTimer
			.set(1.0f, [ctx, debugEnemySpawnArea] {
				const vec2 spawnPos{
					flx::random.nextFloat(debugEnemySpawnArea.left(), debugEnemySpawnArea.right()),
					flx::random.nextFloat(debugEnemySpawnArea.top(), debugEnemySpawnArea.bottom()),
				};
				debugEnemeyBirth(ctx, spawnPos, "enemy");
			})
			.start(-1);
	}

	void EnemySpawnSystem::updateAfterCleanup(const GameCtx& ctx, float dt) {
		static const bool& enableEnemySpawn = app::DebugVariables::emplace<bool>("enableEnemySpawn", true);
		static const float& enemySpawnFreq = app::DebugVariables::emplace<float>("enemySpawnFreq", 1.f);
		if (enableEnemySpawn) {
			ctx.gameState.enemySpawnTimer.setDuration(1.f / enemySpawnFreq);
			ctx.gameState.enemySpawnTimer.update(dt);
		}

		for (auto [e, ebc] : ctx.reg.view<EnemyBirthComponent>()) {
			ebc.remainingTime -= dt;
			if (ebc.remainingTime <= 0) {
				EntityService().kill(ctx, e);
				ctx.factory.createEnemy(ctx, ebc.position);
			}
		}
	}
} // namespace flx::game