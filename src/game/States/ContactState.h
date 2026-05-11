#pragma once
#include"src/ecs/types.h"
#include "src/utils/Vec2.h"
#include"src/utils/Pointer.h"

#include<vector>

struct ContactEvent {
	myecs::entity a{};
	myecs::entity b{};
	nvec2 normal{};
	n_pair<nvec2> points{};
	bool isPhysicsEnabled{};
};

struct ContactSettings {
	bool emitEvent{};
	bool enablePhysics{};
};

struct ContactState {
	std::vector<ContactEvent> events;
};