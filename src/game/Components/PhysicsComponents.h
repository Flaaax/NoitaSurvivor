#pragma once
#include "src/ecs/entity.h"
#include "src/meta/MetaAttributes.h"
#include "src/utils/Vec2.h"
#include <box2d/box2d.h>

struct N_NOINIT BodyArg {
	enum ShapeType { Circle, Box };

	enum BodyType { Static, Kinematic, Dynamic };

	BodyType type = Static;
	bool fixedRotation = false;
	ShapeType shape;

	union {
		nvec2 size{};
		float radius;
	};

	float density = 1.f;
	float friction = 0.f;
	float restitution = 0.f;
	float isSensor = false;		//Enable if it is a projectile
};

struct N_NOINIT BodyComponent {
	b2BodyId body = b2_nullBodyId;
	b2ShapeId shape = b2_nullShapeId;
};

// Rely on DirectionComponent and apply force to the body
struct SmoothMovementComponent {
	float acc = 0;
	float dec = 0;
	float spd = 0; // max speed
};