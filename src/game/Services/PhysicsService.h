#pragma once
#include "src/ecs/entity.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/utils/Pointer.h"

struct GameCtx;

class PhysicsService {
private:
	static BodyComponent& getBody(const GameCtx& ctx, myecs::entity e);

public:
	static void assertValid(const BodyComponent& bc);

	static void assertWorldMutable(const GameCtx& ctx);

	static void destroyBody(const GameCtx& ctx, myecs::entity e);

	static void destroyBody(const GameCtx& ctx, const BodyComponent& bc, myecs::entity e);

	static void createBody(const GameCtx& ctx, myecs::entity e, const BodyArg& arg);

	static bool isValid(const BodyComponent& bc);

	static nvec2 getPosition(const BodyComponent& bc);

	static nvec2 getPosition(const GameCtx& ctx, myecs::entity e);

	static void setPosition(const BodyComponent& bc, nvec2 position);

	static void setPosition(const GameCtx& ctx, myecs::entity e, nvec2 position);

	static void setTransform(const BodyComponent& bc, nvec2 position, float rad);

	static void setRotation(const BodyComponent& bc, float rad);

	static nvec2 getVelocity(const BodyComponent& bc);

	static nvec2 getVelocity(const GameCtx& ctx, myecs::entity e);

	static void setVelocity(const BodyComponent& bc, nvec2 velocity);

	static void setVelocity(const GameCtx& ctx, myecs::entity e, nvec2 velocity);

	static float getRadius(const BodyComponent& bc);

	static float getRadius(const GameCtx& ctx, myecs::entity e);

	static void applyLinearImpulse(const BodyComponent& bc, nvec2 impulse);

	static float getMass(const BodyComponent& bc);
};