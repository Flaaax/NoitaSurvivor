#pragma once
#include "src/utils/EntityPair.h"
#include "src/utils/Vec2.h"
#include <boost/unordered/unordered_flat_map.hpp>

struct ContactEvent {
	nvec2 normal{};
	bool isPhysicsEnabled{};
	bool isSensorEvent{};
};

struct ContactSettings {
	bool emitEvent{};
	bool enablePhysics{};
};

struct ContactState {
	using EventRecord = boost::unordered_flat_map<EntityPair, ContactEvent>;
	EventRecord events;
};
