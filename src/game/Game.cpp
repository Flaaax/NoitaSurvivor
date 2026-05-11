#include "Game.h"
#include"Components/EntityComponents.h"
#include"Components/PhysicsComponents.h"
#include"src/gui/Renderer.h"
#include"src/gui/NWindow.h"
#include"src/utils/Logger.h"
#include"Contact/ContactListener.h"
#include"Contact/ContactFilter.h"
#include"Wands/Wand.h"
#include"Systems/GameSystem.h"
#include"Systems/RenderSystem.h"
#include"Systems/GameStateSystem.h"
#include"Systems/PhysicsSystem.h"
#include"Services/PhysicsBodyService.h"
#include "Systems/ContactSystem.h"
#include "Systems/EntityDestroySystem.h"


Game::Game() {
	Logger::info("Game instance created");
}

Game::~Game() {
	Logger::info("entity count: {}", reg.entity_count());
	Logger::info("component count: {}", reg.component_count());
	Logger::info("max entity count: {}", reg.max_entity_count());
	Logger::info("max component count: {}", reg.max_component_count());

	auto ctx = getContext();
	for (auto [e, bc] : reg.view<BodyComponent>()) {
		PhysicsBodyService().destroyBody(ctx, e);
	}

	reg.reset();

	if (world->GetBodyCount() > 0) {
		Logger::warn("Undestroyed body remaining: {}", world->GetBodyCount());
	}
}

GameCtx Game::getContext() {
	return GameCtx{
		reg,
		*world,
		*factory,
		contactRules,
		state,
		contactState
	};
}

void Game::init() {
	if (isInitialized) {
		throw std::runtime_error("Dont initialize more than once");
	}

	isInitialized = true;
	using namespace Util;

	world = make_unique(new b2World({0.f, 0.f}));
	factory = make_unique(new EntityFactory(*this));

	world->SetContinuousPhysics(true);

	auto ctx = getContext();

	contactListener = make_unique(new GameContactListener(ctx));
	contactFilter = make_unique(new GameContactFilter(ctx));
	world->SetContactListener(contactListener.get());
	world->SetContactFilter(contactFilter.get());

	GameStateSystem().initStates(ctx);
}

void Game::draw(Renderer& rdr) {

	auto ctx = getContext();
	RenderSystem().render(rdr, ctx);
}

void Game::update(float dt) {
	if (state.isPaused) {
		return;
	}

	GameCtx ctx = getContext();

	GameStateSystem().updateBeforePhysics(ctx);
	PhysicsSystem().step(dt, ctx);
	ContactSystem().update(ctx, dt);
	PhysicsSystem().update(dt,ctx);
	GameSystem().update(dt, ctx);

	ctx.gameState.enemySpawnTimer.update(dt);

	EntityDestroySystem().destroyDeadEntities(ctx);

	RenderSystem().update(dt, ctx);
}

void Game::handleEvent(const sf::Event& event) {
	GameCtx ctx = getContext();
	GameSystem().handleEvent(event, ctx);
}