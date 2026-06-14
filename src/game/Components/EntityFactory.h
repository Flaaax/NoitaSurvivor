#pragma once
#include "../../utils/Container/Map.h"
#include "../../utils/Vec2/Vec2.h"
#include "src/ecs/entity.h"
#include "src/meta/ComponentMeta.h"
#include <functional>

namespace flx::game {
	class Game;
	struct GameCtx;

	class EntityFactory {
		using ComponentInitializer = meta::ComponentMeta::ComponentInitializer;
		using Factory = std::function<myecs::entity(const GameCtx& ctx)>;

		StrMap<flx::Vector<ComponentInitializer>> entityInitializers;
		StrMap<Factory> factories;

		EntityFactory(const EntityFactory&) = delete;
		void initEntityComponents();
		void initFactories();

	public:
		EntityFactory();

		myecs::entity createPlayer(const GameCtx& ctx);
		myecs::entity createBullet(const GameCtx& ctx, vec2 position, vec2 velocity);
		myecs::entity createBorder(const GameCtx& ctx, vec2 start, vec2 end);
		myecs::entity createEnemy(const GameCtx& ctx, vec2 pos);
		myecs::entity createExplosion(const GameCtx& ctx, vec2 pos, float radius, float impulse);
		myecs::entity createCollector(const GameCtx& ctx, float radius);
		myecs::entity createMaterial(const GameCtx& ctx, vec2 pos, int value = 1);
		myecs::entity createEnemyBirth(const GameCtx& ctx, vec2 pos, std::string_view name);
	};
} // namespace flx::game