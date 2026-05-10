#pragma once
#include"src/ecs/entity.h"
#include"src/utils/Vec2.h"
#include"src/utils/Container.h"
#include<functional>
#include<vector>


class Game;
struct GameCtx;

class EntityFactory {
private:
	Game& game;
	using ComponentInitializer = std::function<void(GameCtx& ctx, myecs::entity e)>;
	using Factory = std::function<myecs::entity()>;
	Util::StdMap<std::vector<ComponentInitializer>> entityInitializers;
	Util::StdMap<Factory> factories;

	EntityFactory(const EntityFactory&) = delete;
	void initEntityComponents();
	void initFactories();

public:
	explicit EntityFactory(Game& game);

	myecs::entity createPlayer();
	myecs::entity createBullet(const nvec2& position, const nvec2& velocity);
	myecs::entity createBorder(const nvec2& start, const nvec2& end);
	myecs::entity createEnemy(const nvec2& pos);
	myecs::entity createExplosion(const nvec2& pos, float radius, float impulse);
	myecs::entity createCollector(float radius);
	myecs::entity createMaterial(const nvec2& pos, int value = 1);
};
