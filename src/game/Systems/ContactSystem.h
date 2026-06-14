#pragma once
#include "src/game/GameContext.h"

namespace flx::game {
	class ContactSystem {
	public:
		static void handleEvents(const GameCtx& ctx);
		static void updateAfterHandleEvent(const GameCtx& ctx, float dt);
	};
} // namespace flx::game