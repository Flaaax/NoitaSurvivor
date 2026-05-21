#pragma once
#include "ScriptComponent.h"
#include "Trackers.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/global/DebugVariables.h"
#include "src/utils/Lambda.h"
#include "src/utils/Logger.h"

// Only works on player projectile...
class TrackingScript : public Script {
private:
	myecs::entity target;
	nvec2 offset;
	float lengthSquared{};
	float maxForceSize = 15.f;
	float radius;

public:
	explicit TrackingScript(float radius) : radius(radius) {}

	using TrackerFcn = nvec2(nvec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody);

	TrackerFcn* tracker = Trackers::circle;

	void onUpdate(const GameCtx& ctx, myecs::entity self, float dt) override {
		if (!EntityService::isAlive(ctx, self)) {
			return;
		}
		constexpr PhysicsService ps{};
		const auto& proj_body = ctx.reg.get<BodyComponent>(self);

		auto queryCallback = Util::unwrapLambda([this, ctx, self](myecs::entity other) {
			if (!EntityService().isValidAndAlive(ctx, target)) {
				target = other;
				offset = ps.getPosition(ctx, other) - ps.getPosition(ctx, self);
				lengthSquared = offset.lengthSquared();
			} else {
				const nvec2 offset1 = ps.getPosition(ctx, other) - ps.getPosition(ctx, self);
				const float lengthSquared1 = offset1.lengthSquared();
				if (lengthSquared1 < lengthSquared) {
					target = other;
					offset = offset1;
					lengthSquared = lengthSquared1;
				}
			}
			return true;
		});

		ps.queryCircle(ctx,
					   PlayerProjectile,
					   ContactLayerRules::bit(Enemy),
					   ps.getPosition(proj_body), radius,
					   queryCallback.fn,
					   queryCallback.ctx());

		if (!EntityService().isValidAndAlive(ctx, target)) {
			return;
		}

		static const int* i{};
		if (!i) {
			i = DebugVariables::get<int>("tracker");
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
				//tracker = Trackers::seek;
				break;
			case 3:
				//tracker = Trackers::weakSeek;
				break;
			case 4:
				//tracker = Trackers::leadSeek;
				break;
			case 5:
				//tracker = Trackers::lateral;
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