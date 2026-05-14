#include "GameService.h"

#include "PhysicsService.h"
#include "src/ecs/entity.h"
#include "src/game/Components/EntityFactory.h"
#include "src/game/GameContext.h"
#include <src/game/Components/PhysicsComponents.h>
#include <src/utils/Math.h>
#include <src/utils/Random.h>

void GameService::dropMaterial(const GameCtx& ctx, nvec2 pos) {
	const auto arg = Util::random.get<float>(0.f, 2 * Util::PI);
	const auto dir = Util::from_rad<nvec2>(arg);
	const auto material = ctx.factory.createMaterial(ctx, pos);
	const float k = Util::random.getFloat(0.55, 0.75);
	PhysicsService().applyImpulse(ctx, material, dir * k);
}