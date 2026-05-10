#pragma once
#pragma warning(disable:5105)
#include"Components/EntityFactory.h"
#include"src/utils/Macro.h"
#include"src/utils/Singleton.h"
#include"Contact/ContactLayerRules.h"
#include"src/utils/Timer.h"
#include"src/ecs/entity.h"
#include"GameContext.h"


class b2World;
class b2ContactListener;
class Wand;
class Renderer;
class GameContactListener;
class GameContactFilter;
namespace sf { class Event; }


class Game {
	N_DECL_SINGLETON(Game);
	friend class EntityFactory;
	friend class GameContactListener;
	friend class GameScene;
private:
	GameState state;

	//GameContext
	myecs::Registry reg;
	n_unique<b2World> world{};
	n_unique<EntityFactory> factory;
	ContactLayerRules contactRules;

	n_unique<GameContactListener> contactListener;
	n_unique<GameContactFilter> contactFilter;

	bool isInitialized = false;

	explicit Game();
	~Game();
	GameCtx getContext();

	float debug_dt{};	//todo
public:
	void init();
	void draw(Renderer& renderer);
	void update(float dt);

	void handleEvent(const sf::Event& event);

	[[nodiscard]] bool isPaused()const { return state.isPaused; }
	void setPaused(bool paused) { state.isPaused = paused; }
};