#include "PhysicsService.h"
#include "src/game/GameContext.h"
#include "src/utils/Logger.h"
#include <src/game/Components/PhysicsComponents.h>

BodyComponent& PhysicsService::getBody(const GameCtx& ctx, myecs::entity e) {
	return ctx.reg.get<BodyComponent>(e);
}

void PhysicsService::assertValid(const BodyComponent& bc) {
	if (!b2Body_IsValid(bc.body) || !b2Shape_IsValid(bc.shape)) {
		Logger::error_and_throw("Body or shape is invalid");
	}
}

void PhysicsService::assertWorldMutable(const GameCtx& ctx) {
	if (ctx.worldCtx.isStepping) {
		Logger::error_and_throw("World is currently not mutable");
	}
}

void PhysicsService::destroyBody(const GameCtx& ctx, myecs::entity e) {
	destroyBody(ctx, getBody(ctx, e), e);
}

void PhysicsService::destroyBody(const GameCtx& ctx, const BodyComponent& bc, myecs::entity e) {
	assertWorldMutable(ctx);
	assertValid(bc);
	b2DestroyBody(bc.body);
}

void PhysicsService::createBody(const GameCtx& ctx, myecs::entity e, const BodyArg& arg) {
	const auto bc = ctx.reg.try_get<BodyComponent>(e);
	if (!bc) {
		Logger::warn("A BodyComponent is required to create a physical body!");
		return;
	}
	if (b2Body_IsValid(bc->body)) {
		Logger::warn("Body of entity {} already exists!", e.string());
		return;
	}
	if (ctx.worldCtx.isStepping) {
		Logger::warn("Cannot create body when world is stepping!");
		return;
	}

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = static_cast<b2BodyType>(arg.type);
	bodyDef.fixedRotation = arg.fixedRotation;
	bodyDef.userData = reinterpret_cast<void*>(e._entity);

	bc->body = b2CreateBody(ctx.worldCtx.world, &bodyDef);

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.userData = reinterpret_cast<void*>(e._entity);
	shapeDef.density = arg.density;
	shapeDef.material.friction = arg.friction;
	shapeDef.material.restitution = arg.restitution;
	shapeDef.isSensor = arg.isSensor;
	shapeDef.enablePreSolveEvents = true;

	if (arg.shape == BodyArg::Box) {
		const b2Polygon shape = b2MakeBox(arg.size.x / 2.f, arg.size.y / 2.f);
		bc->shape = b2CreatePolygonShape(bc->body, &shapeDef, &shape);
	} else if (arg.shape == BodyArg::Circle) {
		b2Circle shape{};
		shape.radius = arg.radius;
		bc->shape = b2CreateCircleShape(bc->body, &shapeDef, &shape);
	} else if (arg.shape == BodyArg::Segment) {
		const b2Segment shape = {arg.point1, arg.point2};
		bc->shape = b2CreateSegmentShape(bc->body, &shapeDef, &shape);
	} else {
		Logger::warn("Shape type not supported: {}", static_cast<int>(arg.shape));
	}
}

bool PhysicsService::isValid(const BodyComponent& bc) {
	return b2Body_IsValid(bc.body) && b2Shape_IsValid(bc.shape);
}

nvec2 PhysicsService::getPosition(const GameCtx& ctx, myecs::entity e) {
	return getPosition(getBody(ctx, e));
}

nvec2 PhysicsService::getPosition(const BodyComponent& bc) {
	assertValid(bc);
	return b2Body_GetPosition(bc.body);
}

void PhysicsService::setPosition(const GameCtx& ctx, myecs::entity e, nvec2 position) {
	setPosition(getBody(ctx, e), position);
}

void PhysicsService::setTransform(const BodyComponent& bc, nvec2 position, float rad) {
	assertValid(bc);
	b2Body_SetTransform(bc.body, position, b2MakeRot(rad));
}
void PhysicsService::setRotation(const BodyComponent& bc, float rad) {
	assertValid(bc);
	b2Body_SetTransform(bc.body, b2Body_GetPosition(bc.body), b2MakeRot(rad));
}

float PhysicsService::getRotation(const BodyComponent& bc) {
	assertValid(bc);
	const auto rot = b2Body_GetRotation(bc.body);
	return b2Rot_GetAngle(rot);
}

void PhysicsService::setPosition(const BodyComponent& bc, nvec2 position) {
	assertValid(bc);
	const auto rot = b2Body_GetRotation(bc.body);
	b2Body_SetTransform(bc.body, position, rot);
}

nvec2 PhysicsService::getVelocity(const BodyComponent& bc) {
	assertValid(bc);
	return b2Body_GetLinearVelocity(bc.body);
}

nvec2 PhysicsService::getVelocity(const GameCtx& ctx, myecs::entity e) {
	return getVelocity(getBody(ctx, e));
}

void PhysicsService::setVelocity(const BodyComponent& bc, nvec2 velocity) {
	assertValid(bc);
	b2Body_SetLinearVelocity(bc.body, velocity);
}

void PhysicsService::setVelocity(const GameCtx& ctx, myecs::entity e, nvec2 velocity) {
	setVelocity(getBody(ctx, e), velocity);
}

float PhysicsService::getRadius(const BodyComponent& bc) {
	assertValid(bc);

	switch (const b2ShapeType type = b2Shape_GetType(bc.shape)) {
	case b2_circleShape:
		const b2Circle circle = b2Shape_GetCircle(bc.shape);
		return circle.radius;

	case b2_polygonShape:
		const b2Polygon polygon = b2Shape_GetPolygon(bc.shape);
		return polygon.radius;

	default:
		Logger::error_and_throw("Shape type {} not supported", static_cast<int>(type));
	}

	return 0.f;
}

float PhysicsService::getRadius(const GameCtx& ctx, myecs::entity e) {
	return getRadius(getBody(ctx, e));
}

void PhysicsService::applyImpulse(const BodyComponent& bc, nvec2 impulse) {
	assertValid(bc);
	b2Body_ApplyLinearImpulseToCenter(bc.body, impulse, true);
}

void PhysicsService::applyImpulse(const GameCtx& ctx, myecs::entity e, nvec2 impulse) {
	return applyImpulse(getBody(ctx, e), impulse);
}

void PhysicsService::applyForce(const BodyComponent& bc, nvec2 force) {
	assertValid(bc);
	b2Body_ApplyForceToCenter(bc.body, force, true);
}

float PhysicsService::getMass(const BodyComponent& bc) {
	assertValid(bc);
	return b2Body_GetMass(bc.body);
}

void PhysicsService::setType(const BodyComponent& bc, BodyArg::BodyType type) {
	assertValid(bc);
	const auto b2Type = static_cast<b2BodyType>(type);
	b2Body_SetType(bc.body, b2Type);
}