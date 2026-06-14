#include "Game.h"
#include "../app/global/DebugVariables.h"
#include "Components/EntityComponents.h"
#include "Components/PhysicsComponents.h"
#include "Contact/PhysicalContactCallbacks.h"
#include "Services/EntityService.h"
#include "Services/PhysicsService.h"
#include "Systems/ContactSystem.h"
#include "Systems/GameStateSystem.h"
#include "Systems/GameSystem.h"
#include "Systems/LifeTimeSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderSystem.h"
#include "src/app/global/LocManager.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "../utils/Logging/Logger.h"
#include "Systems/EnemySpawnSystem.h"
#include "src/game/Wands/Wand.h"

namespace flx::game {
	Game::Game(flx::app::AppContext appCtx) : appCtx(appCtx) {
		logger = flx::Logger::makeAsync("Game", true);
	}

	Game::~Game() {
		if (!isInitialized)
			return;

		logger.info("entity count: {}", reg.entity_count());
		logger.info("component count: {}", reg.component_count());
		logger.info("max entity count: {}", reg.max_entity_count());
		logger.info("max component count: {}", "not avaliable...");

		const auto ctx = getContext();
		for (auto [e, bc] : reg.view<BodyComponent>()) {
			PhysicsService().destroyBody(ctx, e);
		}

		reg.reset();

		if (int count = b2World_GetCounters(worldCtx.world).bodyCount; count > 0) {
			logger.warn("Undestroyed body remaining: {}", count);
		}

		b2DestroyWorld(worldCtx.world);
	}

	GameCtx Game::getContext() {
		return GameCtx{
			.reg = reg,
			.worldCtx = worldCtx,
			.factory = *factory,
			.contactRules = contactRules,
			.gameState = state,
			.contactState = contactState,
			.scales = scales,
			.appCtx = appCtx,
		};
	}

	void Game::init() {
		if (isInitialized) {
			logger.error_and_throw("Initialized more than once");
		}

		app::LocManager::inst().loadDefaultLanguage();

		// todo move this to another place
		b2SetAssertFcn([](const char* condition, const char* fileName, int lineNumber) {
			flx::logger.error("Box2D assert failed:\n    With condition {}\n    At file {}\n    At lineNumber {}", condition, fileName, lineNumber);
			return 1;
		});

		isInitialized = true;
		using namespace flx;

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.enableContinuous = true;
		worldDef.gravity = {0, 0};
		worldCtx.world = b2CreateWorld(&worldDef);

		factory = makeUnique(new EntityFactory());

		const auto ctx = getContext();

		// contactListener = make_unique(new GameContactListener(ctx));
		// contactFilter = make_unique(new GameContactFilter(ctx));
		// world->SetContactListener(contactListener.get());
		// world->SetContactFilter(contactFilter.get());

		ctxInternal = makeUnique(new GameCtx(getContext()));

		b2World_SetCustomFilterCallback(ctx.worldCtx.world, PhysicalContactCallbacks::filterCallback, ctxInternal.get());
		b2World_SetPreSolveCallback(ctx.worldCtx.world, PhysicalContactCallbacks::presolveCallback, ctxInternal.get());

		GameStateSystem().initGameState(ctx);
		EnemySpawnSystem().setup(ctx);
	}

	void Game::draw(ui::NRenderBuffer& rdr) {
		const auto ctx = getContext();
		RenderSystem().render(rdr, ctx);
	}

	void Game::update(float dt) {
		if (state.isPaused) {
			return;
		}

		const GameCtx ctx = getContext();

		GameStateSystem().updateBeforePhysics(ctx);
		PhysicsSystem().step(ctx, dt);
		ContactSystem().handleEvents(ctx);
		ContactSystem().updateAfterHandleEvent(ctx, dt);
		PhysicsSystem().updateAfterContactSystem(ctx, dt);
		GameSystem().update(ctx, dt);

		static bool& shouldClear = app::DebugVariables::try_emplace<bool>("shouldClearEntities", false);
		if (shouldClear) {
			shouldClear = false;
			EntityService().clearMostEntities(ctx);
		}

		LifeTimeSystem().cleanupDeadEntities(ctx);

		GameSystem().updateAfterCleanup(ctx);
		EnemySpawnSystem().updateAfterCleanup(ctx, dt);

		RenderSystem().update(ctx, dt);
	}

	void Game::handleEvent(const sf::Event& event) {
		const GameCtx ctx = getContext();
		GameSystem().handleEvent(ctx, event);
	}
} // namespace flx::game