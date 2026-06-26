#pragma once
#include "../../../app/global/DebugVariables.h"
#include "ScriptComponent.h"
#include "Trackers.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"

namespace flx::game {
	// Only works on player projectile...
	class TrackingScript : public Script {
	private:
		myecs::entity target;
		vec2 offset;
		//float lengthSquared{};
		float maxForceSize = 15.f;
		float radius;

	public:
		explicit TrackingScript(float radius) : radius(radius) {}

		using TrackerFcn = vec2(vec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody);

		TrackerFcn* tracker = Trackers::circle;

		void onUpdate(const GameCtx& ctx, myecs::entity self, float dt) override {
			if (!EntityService::isAlive(ctx, self)) {
				return;
			}
			constexpr PhysicsService ps{};
			const auto& proj_body = ctx.reg.get<BodyComponent>(self);

			// auto queryCallback = [this, ctx, self](myecs::entity other) {
			// 	if (!EntityService().isValidAndAlive(ctx, target) ||
			// 		EntityService().getLayer(ctx, target) != EntityType::Enemy) {
			// 		target = other;
			// 		offset = ps.getPosition(ctx, other) - ps.getPosition(ctx, self);
			// 		lengthSquared = offset.lengthSquared();
			// 	} else {
			// 		const vec2 offset1 = ps.getPosition(ctx, other) - ps.getPosition(ctx, self);
			// 		const float lengthSquared1 = offset1.lengthSquared();
			// 		if (lengthSquared1 < lengthSquared) {
			// 			target = other;
			// 			offset = offset1;
			// 			lengthSquared = lengthSquared1;
			// 		}
			// 	}
			// 	return true;
			// };

			target = ps.queryNearestEntity(ctx, EntityType::Enemy, ps.getPosition(proj_body), radius, target);
			// ps.queryCircle(ctx,
			// 			   ContactLayerRules::bit(EntityType::Enemy),
			// 			   ps.getPosition(proj_body),
			// 			   radius,
			// 			   queryCallback);

			if (!EntityService().isValidAndAlive(ctx, target) ||
				EntityService().getLayer(ctx, target) != EntityType::Enemy) {
				target = {};
				return;
			}

			offset = ps.getPosition(ctx, target) - ps.getPosition(proj_body);

			static const int* i{};
			if (!i) {
				i = app::DebugVariables::tryGet<int>("tracker");
			}
			if (i) {
				switch (*i) {
				case 0:
					tracker = Trackers::none;
					break;
				case 1:
					tracker = Trackers::circle;
					break;
				case 2:
					// tracker = Trackers::seek;
					break;
				case 3:
					// tracker = Trackers::weakSeek;
					break;
				case 4:
					// tracker = Trackers::leadSeek;
					break;
				case 5:
					// tracker = Trackers::lateral;
					break;
				case 6:
					tracker = Trackers::navigation;
				default:
					break;
				}
			}

			ps.applyForce(proj_body, tracker(offset, proj_body, ctx.reg.get<BodyComponent>(target)));

			target = {};
		}
	};
} // namespace flx::game