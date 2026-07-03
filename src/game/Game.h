#pragma once
#include "Components/EntityFactory.h"
#include "Contact/ContactLayerRules.h"
#include "GameContext.h"
#include "render/RenderScales.h"
#include "src/app/AppCtx.h"
#include "src/ecs/entity.h"
#include "src/utils/Logging/Logger.h"

namespace sf {
	class Event;
}

namespace flx::ui {
	class RenderBuffer;
	struct InputState;
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
		Unique<EntityFactory> factory;
		RenderScales renderStates;
		app::AppCtx appCtx;

		Logger logger;

		Unique<GameCtx> ctxInternal;

		bool isInitialized = false;

	public:
		explicit Game(app::AppCtx appCtx);
		~Game();

		void init();
		void draw(ui::RenderBuffer& rdr);
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