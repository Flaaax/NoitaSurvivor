#pragma once
#include"src/ecs/entity.h"
#include"src/game/Components/PhysicsComponents.h"

struct GameCtx;

class PhysicsBodyService {
public:
	void destroyBody(GameCtx& ctx, myecs::entity e);
	void createBody(GameCtx& ctx, myecs::entity e, const BodyArg& arg);
	void createBody(GameCtx& ctx, myecs::entity e, const b2BodyDef& bodyDef, const b2FixtureDef& fixtureDef);
};
