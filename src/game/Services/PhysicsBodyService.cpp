#include "PhysicsBodyService.h"
#include"src/game/GameContext.h"
#include <src/game/Components/PhysicsComponents.h>
#include"src/utils/Logger.h"


void PhysicsBodyService::destroyBody(GameCtx& ctx, myecs::entity e) {
	auto bc = ctx.reg.try_get<BodyComponent>(e);
	if (bc && bc->body) {
		ctx.world.DestroyBody(bc->body);
		bc->body = nullptr;
	}
	else {
		Logger::warn("Entity {}/{} does not have a body to destroy!", e.id, e.version);
	}
}

void PhysicsBodyService::createBody(GameCtx& ctx, myecs::entity e, const BodyArg& arg) {
	b2BodyDef bodyDef;
	bodyDef.type = static_cast<b2BodyType>(arg.type);
	bodyDef.fixedRotation = arg.fixedRotation;
	b2Shape* shape{};
	b2PolygonShape box;
	b2CircleShape circle;
	if (arg.shape == BodyArg::Box) {
		box.SetAsBox(arg.size.x, arg.size.y);
		shape = &box;
	}
	else if (arg.shape == BodyArg::Circle) {
		circle.m_radius = arg.radius;
		shape = &circle;
	}
	else {
		Logger::warn("Invalid shape to create a body!");
		return;
	}
	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;
	fixtureDef.density = arg.density;
	fixtureDef.friction = arg.friction;
	fixtureDef.restitution = arg.restitution;

	createBody(ctx, e, bodyDef, fixtureDef);
}

void PhysicsBodyService::createBody(GameCtx& ctx, myecs::entity e, const b2BodyDef& bodyDef, const b2FixtureDef& fixtureDef) {
	auto bc = ctx.reg.try_get<BodyComponent>(e);
	if (!bc) {
		Logger::warn("Entity {} does not have BodyComponent, failed to create body!", e.string());
		return;
	}
	if (bc->body) {
		Logger::warn("Body of entity {} already exists!", e.string());
		return;
	}
	//if (fixtureDef.density <=0.001f) Logger::error_throw("Fixture density cannot be zero!");
	if (ctx.world.IsLocked()) {
		Logger::warn("Cannot create body when b2World is locked!");
		return;
	}
	bc->body = ctx.world.CreateBody(&bodyDef);
	bc->body->CreateFixture(&fixtureDef);
	bc->body->GetUserData().pointer = e._entity;
	//if (body->GetMass() < 0.001f)Logger::error_throw("Body mass cannot be zero!");
}
