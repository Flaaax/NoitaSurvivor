#pragma once
#pragma warning(disable : 5105)
#include "Components/EntityFactory.h"
#include "Contact/ContactLayerRules.h"
#include "GameContext.h"
#include "src/ecs/entity.h"
#include "src/utils/Singleton.h"

class Wand;
class Renderer;

namespace sf {
	class Event;
}

struct NInputState;

class Game {
	N_DECL_SINGLETON(Game);
	// friend class EntityFactory;
	friend class GameScene;

private:
	GameState state;
	ContactState contactState;
	WorldCtx worldCtx;
	n_unique<GameCtx> ctxInternal;

	// GameContext
	myecs::Registry reg;
	n_unique<EntityFactory> factory;
	ContactLayerRules contactRules;

	bool isInitialized = false;

	GameCtx getContext();

	explicit Game();
	~Game();

public:
	void init();
	void draw(Renderer& rdr);
	void update(float dt);

	void handleEvent(const sf::Event& event);

	bool isPaused() const {
		return state.isPaused;
	}

	void setPaused(bool paused) {
		state.isPaused = paused;
	}
};