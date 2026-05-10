#include "GameService.h"
#include"src/game/GameContext.h"
#include <src/utils/Random.h>
#include <src/utils/Math.h>
#include <src/game/Components/PhysicsComponents.h>
#include"src/ecs/entity.h"
#include"src/game/Components/EntityFactory.h"

void GameService::dropMaterial(nvec2 pos, GameCtx& ctx) {
	auto arg = Util::random.get<float>(0.f, 2 * Util::PI);
	auto dir = Util::from_rad<nvec2>(arg);
	auto material = ctx.factory.createMaterial(pos);
	auto& c = ctx.reg.get<BodyComponent>(material);
	c.body->ApplyLinearImpulseToCenter(dir * 2.5f, true);
}