#pragma once
#include "../../utils/Container/Map.h"
#include "../../utils/Vec2/Vec2.h"
#include "src/ecs/entity.h"
#include "src/meta/ComponentMeta.h"
#include <functional>
#include <vector>

class Game;
struct GameCtx;

class EntityFactory {
	using ComponentInitializer = ComponentMeta::ComponentInitializer;
	using Factory = std::function<myecs::entity(const GameCtx& ctx)>;

	Util::StrMap<Util::Vector<ComponentInitializer>> entityInitializers;
	Util::StrMap<Factory> factories;

	EntityFactory(const EntityFactory&) = delete;
	void initEntityComponents();
	void initFactories();

public:
	EntityFactory();

	myecs::entity createPlayer(const GameCtx& ctx);
	myecs::entity createBullet(const GameCtx& ctx, nvec2 position, nvec2 velocity);
	myecs::entity createBorder(const GameCtx& ctx, nvec2 start, nvec2 end);
	myecs::entity createEnemy(const GameCtx& ctx, nvec2 pos);
	myecs::entity createExplosion(const GameCtx& ctx, nvec2 pos, float radius, float impulse);
	myecs::entity createCollector(const GameCtx& ctx, float radius);
	myecs::entity createMaterial(const GameCtx& ctx, nvec2 pos, int value = 1);
	myecs::entity createEnemyBirth(const GameCtx& ctx, nvec2 pos, std::string_view name);
};
