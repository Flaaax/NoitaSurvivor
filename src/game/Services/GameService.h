#pragma once
#include "../../utils/Vec2/Vec2.h"

namespace flx::game {
	struct GameCtx;

	class GameService {
	public:
		static void dropMaterial(const GameCtx& ctx, vec2 pos);
	};
} // namespace flx::game