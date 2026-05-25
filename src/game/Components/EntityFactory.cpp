#include "EntityFactory.h"
#include "EntityComponents.h"
#include "PhysicsComponents.h"
#include "Render/RenderComponent.h"
#include "Render/SpriteEffects.h"
#include "src/game/Game.h"
#include "src/game/Services/PhysicsService.h"
#include "src/global/DataManager.h"
#include "src/meta/ComponentMeta.h"

using namespace myecs;

void validateComponentConfig(std::string_view entity, std::string_view name, const json& j) {

	const auto metaData = ComponentMeta::getMetaInfo(name);
	if (!metaData) {
		Logger::warn("Invalid component: {}\n\nfor entity{}", name, entity);
		return;
	}
	for (auto& [fieldName, _] : j.items()) {
		if (!metaData->fields.view().any([&](const auto& field) { return field.name == fieldName; })) {
			Logger::warn("Invalid key: {}\n\tfor component {}\n\tfor entity {}", fieldName, name, entity);
		}
	}
}

void EntityFactory::initEntityComponents() {
	for (auto& [entityType, j] : DataMgr::getEntityComponentData().items()) {
		std::vector<ComponentInitializer> components;
		for (auto& [componentName, jj] : j.items()) {
			validateComponentConfig(entityType, componentName, jj);
			if (const auto gen = ComponentMeta::getInitializerFactory(componentName)) {
				components.emplace_back(gen(jj));
			} else
				Logger::warn("Component generator not found: {}\n\t When initializing entityType: {}", componentName, entityType);
		}
		entityInitializers[entityType] = std::move(components);
	}
}

void EntityFactory::initFactories() {
	for (auto& [entityType, components] : entityInitializers) {
		auto* componentList = &components;
		factories[entityType] = [this, componentList](const GameCtx& ctx) {
			auto e = ctx.reg.create();
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
	Logger::info("Creating player entity...");
	static auto& factory = factories["player"];
	auto e = factory(ctx);
	auto effect = new BouncyMoveEffect({0.8f, 1.2f}, {1.2f, 0.8f}, 0.5f);
	//effect->easing_function = Easing::ease_out_cubic;
	ctx.reg.emplace<SpriteEffectComponent>(e).effectList.emplace_back(effect);

	Logger::info("Player entity created: {}", e.string());
	// Logger::info("Player has BodyComponent: {}",ctx.reg.has<BodyComponent>(e));
	return e;
}

myecs::entity EntityFactory::createBullet(const GameCtx& ctx, const nvec2& position, const nvec2& velocity) {
	// todo
	static auto& factory = factories["bullet"];
	auto e = factory(ctx);
	const auto& body = ctx.reg.get<BodyComponent>(e);
	PhysicsService().setTransform(body, position, Util::to_rad(velocity));
	PhysicsService().setVelocity(body, velocity);
	return e;
}

// Should keep this one
myecs::entity EntityFactory::createBorder(const GameCtx& ctx, const nvec2& start, const nvec2& end) {
	static auto& factory = factories["border"];
	auto e = factory(ctx);
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

myecs::entity EntityFactory::createEnemy(const GameCtx& ctx, const nvec2& pos) {
	static auto& factory = factories["enemy"];
	auto e = factory(ctx);

	PhysicsService().setPosition(ctx, e, pos);

	return e;
}

myecs::entity EntityFactory::createExplosion(const GameCtx& ctx, const nvec2& pos, float radius, float impulse) {
	// Logger::error("Creating explosion entity at position ({}, {}) with radius {} and impulse {}", pos.x, pos.y, radius, impulse);
	static auto& factory = factories["explosion"];
	auto e = factory(ctx);
	auto& reg = ctx.reg;
	reg.emplace<BodyComponent>(e);

	const BodyArg arg{
		.type = BodyArg::Static,
		.fixedRotation = true,
		.shape = BodyArg::Circle,
		.radius = radius,
	};
	PhysicsService().createBody(ctx, e, arg);
	const auto& body = reg.get<BodyComponent>(e);
	PhysicsService().setPosition(ctx, e, pos);

	auto& p = reg.emplace<ProjectileComponent>(e);
	p.pierce = -1;
	p.impulse = impulse;
	p.damage = 4;

	constexpr float lifetime = 0.075f;
	reg.emplace<LifetimeComponent>(e).lifeTimer.start(lifetime);

	reg.emplace<MultiContactComponent>(e);
	ctx.reg.emplace<SpriteEffectComponent>(e).effectList +=
		Util::makeUnique(new Tween({}, {.opacity = 0}, lifetime, 0, Easing::ease_out_quad));
	ctx.reg.emplace<ExplosionComponent>(e);

	return e;
}

myecs::entity EntityFactory::createCollector(const GameCtx& ctx, float radius) {
	auto& reg = ctx.reg;
	auto collector = reg.create();

	reg.emplace<EntityComponent>(collector).layer = ContactLayer::Collector;

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

myecs::entity EntityFactory::createMaterial(const GameCtx& ctx, const nvec2& pos, int value) {
	static auto& factory = factories["material"];
	auto e = factory(ctx);
	PhysicsService().setPosition(ctx, e, pos);

	ctx.reg.emplace<MaterialComponent>(e).value = value;

	return e;
}