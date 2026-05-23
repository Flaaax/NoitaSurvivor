#include "GameService.h"

#include "PhysicsService.h"
#include "src/ecs/entity.h"
#include "src/game/Components/EntityFactory.h"
#include "src/game/GameContext.h"
#include <src/game/Components/PhysicsComponents.h>
#include <src/utils/Math.h>
#include <src/utils/Random.h>

void GameService::dropMaterial(const GameCtx& ctx, nvec2 pos) {
	const auto arg = Util::random.nextVal<float>(0.f, 2 * Util::PI);
	const auto dir = Util::from_rad(arg);
	const auto material = ctx.factory.createMaterial(ctx, pos);
	const float k = Util::random.nextFloat(0.55f, 0.75f);
	PhysicsService().applyImpulse(ctx, material, dir * k);
}