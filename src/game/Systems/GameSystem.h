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
	static void update(float dt, GameCtx& ctx);

	static void handleEvent(const sf::Event& event, GameCtx& ctx);

};

#endif