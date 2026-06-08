#pragma once
#include "src/ecs/entity.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Contact/ContactLayerRules.h"
#include "src/utils/Lambda.h"

struct GameCtx;

class PhysicsService {
private:
	using queryCallbackFcn = bool(myecs::entity e, void* customContext);

	static BodyComponent& getBody(const GameCtx& ctx, myecs::entity e);
	static void queryCircle(const GameCtx& ctx, ContactLayer layer, u64 targetLayers,
							nvec2 center, float radius, queryCallbackFcn* customCallback, void* customContext);

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

	static float getRotation(const BodyComponent& bc);

	static nvec2 getVelocity(const BodyComponent& bc);

	static nvec2 getVelocity(const GameCtx& ctx, myecs::entity e);

	static void setVelocity(const BodyComponent& bc, nvec2 velocity);

	static void setVelocity(const GameCtx& ctx, myecs::entity e, nvec2 velocity);

	static float getRadius(const BodyComponent& bc);

	static float getRadius(const GameCtx& ctx, myecs::entity e);

	static void applyImpulse(const BodyComponent& bc, nvec2 impulse);

	static void applyImpulse(const GameCtx& ctx, myecs::entity e, nvec2 impulse);

	static void applyForce(const BodyComponent& bc, nvec2 force);

	static float getMass(const BodyComponent& bc);

	static void setType(const BodyComponent& bc, BodyArg::BodyType type);

	static void applySoftCollision(const GameCtx& ctx, myecs::entity a, myecs::entity b);

	template <class Callback>
		requires std::is_invocable_r_v<bool, Callback&, myecs::entity>
	static void queryCircle(const GameCtx& ctx, u64 targetLayers, nvec2 center, float radius, Callback&& cbLambda, ContactLayer layer = ContactLayer::All) {
		auto fn = Util::unwrapLambda(std::forward<Callback>(cbLambda));
		PhysicsService::queryCircle(ctx, layer, targetLayers, center, radius, fn.fn, fn.ctx());
	}
};