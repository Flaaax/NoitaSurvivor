#pragma once
#include "../../utils/Functional/Lambda.h"
#include "src/ecs/entity.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Contact/ContactLayerRules.h"
#include "src/utils/Functional/FuncRef.h"

namespace flx::game {
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

		static vec2 getPosition(const BodyComponent& bc);

		static vec2 getPosition(const GameCtx& ctx, myecs::entity e);

		static void setPosition(const BodyComponent& bc, vec2 position);

		static void setPosition(const GameCtx& ctx, myecs::entity e, vec2 position);

		static void setTransform(const BodyComponent& bc, vec2 position, float rad);

		static void setRotation(const BodyComponent& bc, float rad);

		static float getRotation(const BodyComponent& bc);

		static vec2 getVelocity(const BodyComponent& bc);

		static vec2 getVelocity(const GameCtx& ctx, myecs::entity e);

		static void setVelocity(const BodyComponent& bc, vec2 velocity);

		static void setVelocity(const GameCtx& ctx, myecs::entity e, vec2 velocity);

		static float getRadius(const BodyComponent& bc);

		static float getRadius(const GameCtx& ctx, myecs::entity e);

		static void applyImpulse(const BodyComponent& bc, vec2 impulse);

		static void applyImpulse(const GameCtx& ctx, myecs::entity e, vec2 impulse);

		static void applyForce(const BodyComponent& bc, vec2 force);

		static float getMass(const BodyComponent& bc);

		static void setType(const BodyComponent& bc, BodyArg::BodyType type);

		static void applySoftCollision(const GameCtx& ctx, myecs::entity a, myecs::entity b);

		static void queryCircle(const GameCtx& ctx, LayerRules::Mask targetLayers, vec2 center, float radius, FuncRef<bool(myecs::entity)> callback, EntityType layer = EntityType::All);
		static myecs::entity queryNearestEntity(const GameCtx& ctx, LayerRules::Mask targetLayers, vec2 center, float radius, myecs::entity preferred = {}, bool preferIfInside = false);
	};
} // namespace flx::game