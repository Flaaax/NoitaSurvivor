#pragma once
#ifndef N_SYSTEM_H
#define N_SYSTEM_H

namespace sf {
	class Event;
}
struct GameCtx;
struct nvec2;

class GameSystem {
public:
	static void update(const GameCtx& ctx, float dt);

	static void handleEvent(const GameCtx& ctx, const sf::Event& event);

	// cleanup enemy states and directions
	static void updateAfterCleanup(const GameCtx& ctx);
};

#endif