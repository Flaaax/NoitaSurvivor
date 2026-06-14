#pragma once
#include "States/ContactState.h"
#include "States/GameState.h"
#include "src/app/AppContext.h"
#include "src/ecs/entity.h"

// namespace flx::ui {
// 	struct NInputState;
// }

namespace flx::game {
	struct GameRenderScales;
	class EntityFactory;
	struct ContactLayerRules;

	struct WorldCtx {
		b2WorldId world = b2_nullWorldId;
		mutable bool isStepping{}; // Do not modify outside PhysicsSystem!
	};

	// Lightweight, copyable reference of the game context
	struct GameCtx {
		myecs::Registry& reg;
		const WorldCtx& worldCtx;
		EntityFactory& factory;
		const ContactLayerRules& contactRules;
		GameState& gameState;
		ContactState& contactState;
		GameRenderScales& scales;
		const flx::app::AppContext& appCtx;
	};
} // namespace flx::game