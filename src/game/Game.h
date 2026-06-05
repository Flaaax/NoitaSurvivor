#pragma once
#pragma warning(disable : 5105)
#include "Components/EntityFactory.h"
#include "Contact/ContactLayerRules.h"
#include "GameContext.h"
#include "render/GameRenderScales.h"
#include "src/app/AppContext.h"
#include "src/ecs/entity.h"

class Wand;
class NRenderBuffer;

namespace sf {
	class Event;
}

struct NInputState;

class Game {
	friend class GameScene;

private:
	// GameContext
	GameState state;
	ContactState contactState;
	ContactLayerRules contactRules;
	WorldCtx worldCtx;
	myecs::Registry reg;
	n_unique<EntityFactory> factory;
	GameRenderScales scales;
	flx::app::AppContext appCtx;

	flx::Logger logger;

	n_unique<GameCtx> ctxInternal;

	bool isInitialized = false;

	GameCtx getContext();

	explicit Game(flx::app::AppContext appCtx);
	~Game();

public:
	void init();
	void draw(NRenderBuffer& rdr);
	void update(float dt);

	void handleEvent(const sf::Event& event);

	bool isPaused() const {
		return state.isPaused;
	}

	void setPaused(bool paused) {
		state.isPaused = paused;
	}
};