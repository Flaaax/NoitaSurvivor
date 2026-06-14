#pragma once
#pragma warning(disable : 5105)
#include "Components/EntityFactory.h"
#include "Contact/ContactLayerRules.h"
#include "GameContext.h"
#include "render/GameRenderScales.h"
#include "src/app/AppContext.h"
#include "src/ecs/entity.h"

namespace sf {
	class Event;
}

namespace flx::ui {
	class NRenderBuffer;
	struct NInputState;
} // namespace flx::ui

namespace flx::game {
	class Wand;

	class Game {
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

	public:
		explicit Game(flx::app::AppContext appCtx);
		~Game();

		void init();
		void draw(ui::NRenderBuffer& rdr);
		void update(float dt);

		void handleEvent(const sf::Event& event);

		bool isPaused() const {
			return state.isPaused;
		}

		void setPaused(bool paused) {
			state.isPaused = paused;
		}

		GameCtx getContext();
	};
} // namespace flx::game