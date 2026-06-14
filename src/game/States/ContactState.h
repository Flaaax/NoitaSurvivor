#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/EntityPair.h"
#include <boost/unordered/unordered_flat_map.hpp>

namespace flx::game {
	struct ContactEvent {
		vec2 normal{};
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
} // namespace flx::game