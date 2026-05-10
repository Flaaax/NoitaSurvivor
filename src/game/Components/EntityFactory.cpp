#include"EntityFactory.h"
#include"EntityComponents.h"
#include"PhysicsComponents.h"
#include"src/game/Game.h"
#include"Render/RenderComponent.h"
#include"src/global/DataManager.h"
#include"Render/SpriteEffects.h"
#include"src/meta/ComponentMeta.h"
#include"src/game/Services/PhysicsBodyService.h"

using namespace myecs;

void EntityFactory::initEntityComponents() {
	for (auto& [entityType, j] : DataMgr::getEntityComponentData().items()) {
		std::vector<ComponentInitializer> components;
		for (auto& [componentName, jj] : j.items()) {
			if (auto gen = ComponentMeta::getInitializerFactory(componentName)) {
				components.emplace_back(gen(jj));
			}
			else Logger::warn("Component generator not found: {}\n\t When initializing entityType: {}", componentName, entityType);
		}
		entityInitializers[entityType] = std::move(components);
	}
}

void EntityFactory::initFactories() {
	for (auto& [entityType, components] : entityInitializers) {
		auto* componentList = &components;
		factories[entityType] = [this, componentList]() {
			auto ctx = game.getContext();
			auto e = ctx.reg.create();
			for (auto& component : *componentList) {
				component(ctx, e);
			}
			return e;
		};
	}
}

EntityFactory::EntityFactory(Game& game) :game(game) {
	initEntityComponents();
	initFactories();
}

//generally, do not set anything to sensor
myecs::entity EntityFactory::createPlayer() {
	Logger::info("Creating player entity...");
	static auto& factory = factories["player"];
	auto e = factory();
	auto ctx = game.getContext();
	auto effect = new BouncyMoveEffect({ 1.2f,0.8f }, { 0.8f,1.2f }, 0.5f);
	ctx.reg.emplace<SpriteEffectComponent>(e)
		.effectList
		.emplace_back(effect);

	Logger::info("Player entity created: {}", e._entity);
	return e;
}

myecs::entity EntityFactory::createBullet(const nvec2& position, const nvec2& velocity) {
	//todo ÈÃbullet¸úËæÍæ¼ÒËÙ¶È
	static auto& factory = factories["bullet"];
	auto e = factory();
	auto ctx = game.getContext();
	auto& body = ctx.reg.get<BodyComponent>(e);

	body.body->SetTransform(position, Util::to_rad(velocity));
	body.setVelocity(velocity);

	return e;
}

//Should keep this one
myecs::entity EntityFactory::createBorder(const nvec2& start, const nvec2& end) {
	static auto& factory = factories["border"];
	auto e = factory();
	auto ctx = game.getContext();
	auto& reg = ctx.reg;

	reg.emplace<BodyComponent>(e);
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;

	b2EdgeShape edge;
	edge.SetTwoSided(start, end);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &edge;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.isSensor = false;

	PhysicsBodyService().createBody(ctx, e, bodyDef, fixtureDef);

	return e;
}

myecs::entity EntityFactory::createEnemy(const nvec2& pos) {
	static auto& factory = factories["enemy"];
	auto e = factory();
	auto ctx = game.getContext();

	auto& body = ctx.reg.get<BodyComponent>(e);
	body.setPosition(pos);

	return e;
}

myecs::entity EntityFactory::createExplosion(const nvec2& pos, float radius, float impulse = 30) {
	//Logger::error("Creating explosion entity at position ({}, {}) with radius {} and impulse {}", pos.x, pos.y, radius, impulse);
	static auto& factory = factories["explosion"];
	auto e = factory();
	auto ctx = game.getContext();
	auto& reg = ctx.reg;
	reg.emplace<BodyComponent>(e);

	BodyArg arg{
		.type = BodyArg::Static,
		.fixedRotation = true,
		.shape = BodyArg::Circle,
		.radius = radius
	};
	PhysicsBodyService().createBody(ctx, e, arg);
	auto& body = reg.get<BodyComponent>(e);
	body.body->SetTransform(pos, 0.f);

	auto& p = reg.emplace<ProjectileComponent>(e);
	p.pierce = -1;
	p.impulse = impulse;
	p.damage = 4;

	float lifetime = 0.075f;
	reg.emplace<LifetimeComponent>(e).lifeTimer.start(lifetime);

	reg.emplace<MultiContactComponent>(e);
	ctx.reg.emplace<SpriteEffectComponent>(e).effectList += Util::make_unique(new Transition({}, { .opacity = 0 }, lifetime, Easing::ease_out_quad));

	return e;
}

myecs::entity EntityFactory::createCollector(float radius) {
	auto ctx = game.getContext();
	auto& reg = ctx.reg;
	auto collector = reg.create();

	reg.emplace<BodyComponent>(collector);
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.fixedRotation = true;

	b2CircleShape shape;
	shape.m_radius = radius;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.filter.categoryBits = (uint16)ContactMaskFilter::Collectable;
	fixtureDef.filter.maskBits = (uint16)ContactMaskFilter::Collectable;

	PhysicsBodyService().createBody(ctx, collector, bodyDef, fixtureDef);

	reg.emplace<EntityComponent>(collector).layer = ContactLayer::Collectable;
	//reg.emplace<RenderComponent>(collector).rp = EntityRenderManager::getRender<ExplosionRender>();

	return collector;
}

myecs::entity EntityFactory::createMaterial(const nvec2& pos, int value) {
	static auto& factory = factories["material"];
	auto e = factory();
	auto ctx = game.getContext();
	auto& reg = ctx.reg;

	auto& body = reg.get<BodyComponent>(e);
	b2Filter filter;
	filter.categoryBits = (uint16)ContactMaskFilter::Collectable;
	filter.maskBits = (uint16)ContactMaskFilter::Collectable;
	body.body->GetFixtureList()->SetFilterData(filter);

	body.setPosition(pos);

	reg.emplace<MaterialComponent>(e).value = value;

	return e;
}

