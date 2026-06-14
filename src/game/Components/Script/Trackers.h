#pragma once

#include <algorithm>
#include <cmath>

#include "../../../utils/Vec2/Vec2.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Mat22.h"

namespace flx::game {
	struct Trackers {
	private:
		static constexpr float maxForce = 15.f;

		static vec2 clampLength(vec2 v, float maxLength) {
			const float lenSq = v.lengthSquared();
			if (lenSq < math::n_epsilon_2) {
				return {};
			}

			if (lenSq <= maxLength * maxLength) {
				return v;
			}

			return v * (maxLength / std::sqrt(lenSq));
		}

		static vec2 normalForceToOffset(vec2 offset, const BodyComponent& projBody, float responseTime) {
			PhysicsService ps{};

			const vec2 velocity = ps.getVelocity(projBody);
			const float speed = velocity.length();

			if (
				offset.lengthSquared() < math::n_epsilon_2 ||
				speed < math::n_epsilon ||
				responseTime < math::n_epsilon) {
				return {};
			}

			const vec2 dir = velocity / speed;
			const vec2 targetDir = offset.normalized();
			const vec2 leftNormal = {-dir.y, dir.x};

			const float side = dir.cross(targetDir);

			if (std::abs(side) < math::n_epsilon) {
				return {};
			}

			const float mass = ps.getMass(projBody);
			const float forceSize = mass * speed * side / responseTime;

			return clampLength(leftNormal * forceSize, maxForce);
		}

	public:
		static vec2 none(vec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody) {
			return {};
		}

		static vec2 circle(vec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody) {
			PhysicsService ps{};

			const vec2 velocity = ps.getVelocity(projBody);
			const float speedSquared = velocity.lengthSquared();

			if (
				offset.lengthSquared() < math::n_epsilon_2 ||
				speedSquared < math::n_epsilon_2) {
				return {};
			}

			const vec2 dir = velocity.normalized();
			const mat22 rot = {dir.y, -dir.x, dir.x, dir.y};
			const vec2 rotatedOffset = rot * offset;

			// Target is in front or behind of the projectile
			if (std::abs(rotatedOffset.x) < math::n_epsilon) {
				return {};
			}

			const float radius =
				rotatedOffset.lengthSquared() /
				(2.f * std::abs(rotatedOffset.x));

			if (radius < math::n_epsilon) {
				return {};
			}

			const vec2 forceDir = vec2{dir.y, -dir.x} * (rotatedOffset.x > 0.f ? 1.f : -1.f);

			float forceSize = speedSquared * ps.getMass(projBody) / radius;
			forceSize = std::min(forceSize, maxForce);

			return forceDir * forceSize;
		}

		static vec2 navigation(vec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody) {
			PhysicsService ps{};

			const vec2 projVelocity = ps.getVelocity(projBody);
			const vec2 targetVelocity = ps.getVelocity(targetBody);

			const float speed = projVelocity.length();
			const float offsetLengthSquared = offset.lengthSquared();

			if (offsetLengthSquared < math::n_epsilon_2 || speed < math::n_epsilon) {
				return {};
			}

			const vec2 relativeVelocity = targetVelocity - projVelocity;
			const float offsetLength = std::sqrt(offsetLengthSquared);
			const float closingSpeed = -offset.dot(relativeVelocity) / offsetLength;

			if (closingSpeed <= 0.f) {
				constexpr float fallbackResponseTime = 0.30f;
				return normalForceToOffset(offset, projBody, fallbackResponseTime);
			}

			const float lineOfSightRate = offset.cross(relativeVelocity) / offsetLengthSquared;

			if (std::abs(lineOfSightRate) < math::n_epsilon) {
				return {};
			}

			constexpr float navigationGain = 3.f;

			const vec2 dir = projVelocity / speed;
			const vec2 leftNormal = {-dir.y, dir.x};

			const float accelerationSize = navigationGain * closingSpeed * lineOfSightRate;
			const vec2 force = leftNormal * (accelerationSize * ps.getMass(projBody));

			return clampLength(force, maxForce);
		}
	};
} // namespace flx::game