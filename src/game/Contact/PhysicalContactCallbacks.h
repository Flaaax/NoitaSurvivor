#pragma once
#include "src/ecs/entity.h"
#include "src/game/GameContext.h"

#include <box2d/types.h>

namespace flx::game {
	// warning: If the world step is multithreaded, all of those callbacks should be thread-safe!	Currently: single threaded
	class PhysicalContactCallbacks {
	public:
		static bool filter(const GameCtx& ctx, myecs::entity a, myecs::entity b);
		static bool filterCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void* context);
		static bool presolveCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);
	};
} // namespace flx::game