#include "EntityFactory.h"
#include "EntityComponents.h"
#include "PhysicsComponents.h"
#include "Render/RenderComponent.h"
#include "Render/SpriteEffects.h"
#include "src/app/global/Loader.h"
// #include "src/game/Game.h"
#include "src/game/GameContext.h"
#include "src/game/Services/PhysicsService.h"
#include "src/meta/ComponentMeta.h"
#include "src/utils/Container/View.h"
#include "src/utils/Fon/Fon.h"
#include "src/utils/Math.h"

namespace flx::game {
	using namespace myecs;
	using namespace meta;

	static Logger logger = Logger::makeAsync("EntityFactory");

	void validateComponentConfig(std::string_view entity, std::string_view name, const Fon& j) {
		const auto metaData = meta::ComponentMeta::getMetaInfo(name);
		if (!metaData) {
			logger.warn("Invalid component: {}\n\nfor entity{}", name, entity);
			return;
		}
		for (const auto fieldName : j.items() | std::views::keys) {
			if (!view::all(metaData->fields).any([&](const auto& field) { return field.name == fieldName; })) {
				logger.warn("Invalid key: {}\n\tfor component {}\n\tfor entity {}", fieldName, name, entity);
			}
		}
	}

	void EntityFactory::initEntityComponents() {
		for (auto& [entityType, j] : app::Loader::loadFonFile("data/component/entity.fon", true)->items()) {
			Vector<ComponentInitializer> components;
			for (auto& [componentName, jj] : j.items()) {
				validateComponentConfig(entityType, componentName, jj);
				if (const auto gen = meta::ComponentMeta::getInitializerFactory(componentName)) {
					components.emplace_back(gen(jj));
				} else
					logger.warn("Component generator not found: {}\n\t When initializing entityType: {}", componentName, entityType);
			}
			entityInitializers[entityType] = std::move(components);
		}
	}

	void EntityFactory::initFactories() {
		for (auto& [entityType, components] : entityInitializers) {
			// Kinda weird to use pointer here?
			auto* componentList = &components;
			factories[entityType] = [this, componentList](const GameCtx& ctx) {
				const auto e = ctx.reg.create();
				for (auto& component : *componentList) {
					component(ctx, e);
				}
				return e;
			};
		}
	}

	EntityFactory::EntityFactory() {
		initEntityComponents();
		initFactories();
	}

	// generally, do not set anything to sensor
	myecs::entity EntityFactory::createPlayer(const GameCtx& ctx) {
		static auto& factory = factories["player"];
		const auto e = factory(ctx);
		// auto effect = new BouncyMoveEffect({0.8f, 1.2f}, {1.2f, 0.8f}, 0.5f);
		// ctx.reg.emplace<SpriteEffectComponent>(e).effectList.emplace_back(effect);

		logger.info("Player entity: {}", e.string());
		// Logger::info("Player has BodyComponent: {}",ctx.reg.has<BodyComponent>(e));
		return e;
	}

	myecs::entity EntityFactory::createBullet(const GameCtx& ctx, vec2 position, vec2 velocity) {
		// todo
		static auto& factory = factories["bullet"];
		const auto e = factory(ctx);
		const auto& body = ctx.reg.get<BodyComponent>(e);
		PhysicsService().setTransform(body, position, velocity.rad());
		PhysicsService().setVelocity(body, velocity);
		return e;
	}

	// Should keep this one
	myecs::entity EntityFactory::createBorder(const GameCtx& ctx, vec2 start, vec2 end) {
		static auto& factory = factories["border"];
		const auto e = factory(ctx);
		auto& reg = ctx.reg;

		reg.emplace<BodyComponent>(e);

		const BodyArg arg{
			.type = BodyArg::Static,
			.shape = BodyArg::Segment,
			.point1 = start,
			.point2 = end,
			.friction = 0.0f,
			.restitution = 0.0f,
		};

		PhysicsService().createBody(ctx, e, arg);

		return e;
	}

	myecs::entity EntityFactory::createEnemy(const GameCtx& ctx, vec2 pos) {
		static auto& factory = factories["enemy"];
		const auto e = factory(ctx);

		PhysicsService().setPosition(ctx, e, pos);

		return e;
	}

	myecs::entity EntityFactory::createExplosion(const GameCtx& ctx, vec2 pos, float radius, float impulse) {
		// Logger::error("Creating explosion entity at position ({}, {}) with radius {} and impulse {}", pos.x, pos.y, radius, impulse);
		static auto& factory = factories["explosion"];
		const auto e = factory(ctx);
		auto& reg = ctx.reg;
		reg.emplace<BodyComponent>(e);

		const BodyArg arg{
			.type = BodyArg::Static,
			.fixedRotation = true,
			.shape = BodyArg::Circle,
			.radius = radius,
		};
		PhysicsService().createBody(ctx, e, arg);
		// const auto& body = reg.get<BodyComponent>(e);
		PhysicsService().setPosition(ctx, e, pos);

		auto& p = reg.emplace<ProjectileComponent>(e);
		p.pierce = -1;
		p.impulse = impulse;
		p.damage = 4;

		constexpr float lifetime = 0.075f;
		reg.emplace<LifetimeComponent>(e).lifeTimer.start(lifetime);

		reg.emplace<MultiContactComponent>(e);
		ctx.reg.emplace<SpriteEffectComponent>(e).effectList +=
			makeUnique(new Tween({}, {.opacity = 0}, lifetime, 0, Easing::ease_out_quad));
		ctx.reg.emplace<ExplosionComponent>(e);

		return e;
	}

	myecs::entity EntityFactory::createCollector(const GameCtx& ctx, float radius) {
		auto& reg = ctx.reg;
		const auto collector = reg.create();

		reg.emplace<EntityComponent>(collector).type = EntityType::Collector;

		reg.emplace<BodyComponent>(collector);

		const BodyArg arg{
			.type = BodyArg::Static,
			.fixedRotation = true,
			.shape = BodyArg::Circle,
			.radius = radius,
			.density = 0.0f,
			.friction = 0.0f,
			.restitution = 0.0f,
		};

		PhysicsService().createBody(ctx, collector, arg);

		// reg.emplace<RenderComponent>(collector).rp = EntityRenderManager::getRender<ExplosionRender>();

		return collector;
	}

	myecs::entity EntityFactory::createMaterial(const GameCtx& ctx, vec2 pos, int value) {
		static auto& factory = factories["material"];
		const auto e = factory(ctx);
		PhysicsService().setPosition(ctx, e, pos);

		ctx.reg.emplace<MaterialComponent>(e).value = value;

		return e;
	}

	myecs::entity EntityFactory::createEnemyBirth(const GameCtx& ctx, vec2 pos, std::string_view name) {
		static auto& factory = factories["enemy_birth"];
		const auto e = factory(ctx);
		const auto& ebc = ctx.reg.emplace<EnemyBirthComponent>(e, pos, std::string(name));
		float dur = ebc.remainingTime;
		ctx.reg.get<SpriteComponent>(e).position = pos;

		ctx.reg.get_or_emplace<SpriteEffectComponent>(e).effectList +=
			std::make_unique<Tween>(EffectState{.opacity = 0.05f},
									EffectState{},
									dur,
									0,
									[](float t) { return Easing::ease_in_out_quad(Easing::ping_pong(t)); });

		return e;
	}
} // namespace flx::game