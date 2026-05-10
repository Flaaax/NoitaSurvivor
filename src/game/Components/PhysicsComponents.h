#pragma once
#include<box2d/box2d.h>
#include"src/ecs/entity.h"
#include"src/utils/Vec2.h"
#include"src/meta/MetaAttributes.h"

struct N_NOINIT BodyArg {
	enum ShapeType {
		Circle,
		Box
	};
	enum BodyType {
		Static,
		Kinematic,
		Dynamic
	};
	BodyType type = Static;
	bool fixedRotation = false;
	ShapeType shape;
	union {
		nvec2 size;
		float radius = 0.f;
	};
	float density = 1.f;
	float friction = 0.f;
	float restitution = 0.f;
};

struct N_NOINIT BodyComponent {
	b2Body* body{};

	BodyComponent() {}
	BodyComponent(BodyComponent&& other)noexcept :body(other.body) { other.body = nullptr; }

	void setPosition(const nvec2& position) {
		body->SetTransform(position.as_bvec2, body->GetAngle());
	}
	void setVelocity(const nvec2& velocity) {
		body->SetLinearVelocity(velocity.as_bvec2);
	}
	nvec2 getPosition()const {
		return body->GetPosition();
	}
	nvec2 getVelocity()const {
		return body->GetLinearVelocity();
	}

	float getRadius()const {
		auto shape = body->GetFixtureList()->GetShape();
		switch (shape->m_type) {
			case b2Shape::e_circle:
			case b2Shape::e_polygon:
				return shape->m_radius;
		}
		throw std::runtime_error("Shape type not supported");
	}
};

//Rely on DirectionComponent and apply force to the body
struct SmoothMovementComponent {
	float acc = 0;
	float dec = 0;
	float spd = 0;				//max speed
};