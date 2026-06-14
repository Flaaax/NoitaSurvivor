#pragma once

namespace sf {
	class Event;
}

namespace flx::game {
	struct GameCtx;

	class GameSystem {
	public:
		static void update(const GameCtx& ctx, float dt);

		static void handleEvent(const GameCtx& ctx, const sf::Event& event);

		// cleanup enemy states and directions
		static void updateAfterCleanup(const GameCtx& ctx);
	};
} // namespace flx::game
