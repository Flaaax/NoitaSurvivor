#include "GameService.h"

#include "PhysicsService.h"
#include "src/ecs/entity.h"
#include "src/game/Components/EntityFactory.h"
#include "src/game/GameContext.h"
#include <src/game/Components/PhysicsComponents.h>
#include <src/utils/Math.h>
#include <src/utils/Random.h>

namespace flx::game {
	void GameService::dropMaterial(const GameCtx& ctx, vec2 pos) {
		const auto arg = flx::random.nextVal<float>(0.f, 2 * flx::math::pi);
		const auto dir = vec2::rad(arg);
		const auto material = ctx.factory.createMaterial(ctx, pos);
		const float k = flx::random.nextFloat(0.55f, 0.75f);
		PhysicsService().applyImpulse(ctx, material, dir * k);
	}
} // namespace flx::game