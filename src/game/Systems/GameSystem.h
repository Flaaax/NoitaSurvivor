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
	void update(float dt, GameCtx& ctx);
	void handleEvent(const sf::Event& event, GameCtx& ctx);

};

#endif