#pragma once

#include <algorithm>
#include <cmath>

#include "../../../utils/Vec2/Vec2.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Mat22.h"

struct Trackers {
private:
	static constexpr float maxForce = 15.f;

	static nvec2 clampLength(nvec2 v, float maxLength) {
		const float lenSq = v.lengthSquared();
		if (lenSq < nmath::n_epsilon_2) {
			return {};
		}

		if (lenSq <= maxLength * maxLength) {
			return v;
		}

		return v * (maxLength / std::sqrt(lenSq));
	}

	static nvec2 normalForceToOffset(nvec2 offset, const BodyComponent& projBody, float responseTime) {
		PhysicsService ps{};

		const nvec2 velocity = ps.getVelocity(projBody);
		const float speed = velocity.length();

		if (
			offset.lengthSquared() < nmath::n_epsilon_2 ||
			speed < nmath::n_epsilon ||
			responseTime < nmath::n_epsilon) {
			return {};
		}

		const nvec2 dir = velocity / speed;
		const nvec2 targetDir = offset.normalized();
		const nvec2 leftNormal = {-dir.y, dir.x};

		const float side = dir.cross(targetDir);

		if (std::abs(side) < nmath::n_epsilon) {
			return {};
		}

		const float mass = ps.getMass(projBody);
		const float forceSize = mass * speed * side / responseTime;

		return clampLength(leftNormal * forceSize, maxForce);
	}
public:
	static nvec2 none(nvec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody) {
		return {};
	}

	static nvec2 circle(nvec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody) {
		PhysicsService ps{};

		const nvec2 velocity = ps.getVelocity(projBody);
		const float speedSquared = velocity.lengthSquared();

		if (
			offset.lengthSquared() < nmath::n_epsilon_2 ||
			speedSquared < nmath::n_epsilon_2) {
			return {};
		}

		const nvec2 dir = velocity.normalized();
		const nmat22 rot = {dir.y, -dir.x, dir.x, dir.y};
		const nvec2 rotatedOffset = rot * offset;

		// Target is in front or behind of the projectile
		if (std::abs(rotatedOffset.x) < nmath::n_epsilon) {
			return {};
		}

		const float radius =
			rotatedOffset.lengthSquared() /
			(2.f * std::abs(rotatedOffset.x));

		if (radius < nmath::n_epsilon) {
			return {};
		}

		const nvec2 forceDir = nvec2{dir.y, -dir.x} * (rotatedOffset.x > 0.f ? 1.f : -1.f);

		float forceSize = speedSquared * ps.getMass(projBody) / radius;
		forceSize = std::min(forceSize, maxForce);

		return forceDir * forceSize;
	}

	static nvec2 navigation(nvec2 offset, const BodyComponent& projBody, const BodyComponent& targetBody) {
		PhysicsService ps{};

		const nvec2 projVelocity = ps.getVelocity(projBody);
		const nvec2 targetVelocity = ps.getVelocity(targetBody);

		const float speed = projVelocity.length();
		const float offsetLengthSquared = offset.lengthSquared();

		if (offsetLengthSquared < nmath::n_epsilon_2 || speed < nmath::n_epsilon) {
			return {};
		}

		const nvec2 relativeVelocity = targetVelocity - projVelocity;
		const float offsetLength = std::sqrt(offsetLengthSquared);
		const float closingSpeed = -offset.dot(relativeVelocity) / offsetLength;

		if (closingSpeed <= 0.f) {
			constexpr float fallbackResponseTime = 0.30f;
			return normalForceToOffset(offset, projBody, fallbackResponseTime);
		}

		const float lineOfSightRate = offset.cross(relativeVelocity) / offsetLengthSquared;

		if (std::abs(lineOfSightRate) < nmath::n_epsilon) {
			return {};
		}

		constexpr float navigationGain = 3.f;

		const nvec2 dir = projVelocity / speed;
		const nvec2 leftNormal = {-dir.y, dir.x};

		const float accelerationSize = navigationGain * closingSpeed * lineOfSightRate;
		const nvec2 force = leftNormal * (accelerationSize * ps.getMass(projBody));

		return clampLength(force, maxForce);
	}
};