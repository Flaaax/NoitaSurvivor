#include "ComponentMeta.h"
#include "CustomFieldParser.h"
#include "EnumMeta.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Components/Render/RenderComponent.h"
#include "src/game/GameContext.h"
#include "src/game/Services/PhysicsService.h"
#include <src/global/SpriteManager.h>

template <class T>
void json_init_enum(T& e, std::string_view name, const json& j, std::string_view key) {
	const auto s = json_parse<std::string>(j, key);
	if (!s)
		return;
	auto ee = EnumMeta::try_get<T>(name, *s);
	if (ee) {
		e = *ee;
	}
}

ComponentMeta::ComponentMeta() {
	initGeneratedMetaInfo();
	initGeneratedComponentInitializers();
	initCustomComponentInitializers();
}

void ComponentMeta::initCustomComponentInitializers() {
	componentInitializerFactories["BodyComponent"] = [](const json& j) -> ComponentInitializer {
		BodyArg arg;
		json_init_enum(arg.type, "BodyType", j, "type");
		json_init_field(arg.fixedRotation, j, "fixedRotation");
		json_init_enum(arg.shape, "ShapeType", j, "shape");
		json_init_field(arg.size, j, "size");
		json_init_field(arg.radius, j, "radius");
		json_init_field(arg.density, j, "density");
		json_init_field(arg.friction, j, "friction");
		json_init_field(arg.restitution, j, "restitution");
		json_init_field(arg.isSensor, j, "isSensor");
		// Logger::info("Density = {}", arg.density);
		return [=](const GameCtx& ctx, myecs::entity e) {
			ctx.reg.get_or_emplace<BodyComponent>(e);
			PhysicsService().createBody(ctx, e, arg);
			// Logger::info("Created body for entity {}", e.string());
			// Logger::info("Entity {} has BodyComponent: {}", e.string(), ctx.reg.has<BodyComponent>(e));
		};
	};

	{
		Util::Vector<std::string_view> keys = {
			"type",
			"fixedRotation",
			"shape",
			"size",
			"radius",
			"density",
			"friction",
			"restitution",
			"isSensor",
		};

		componentMetaInfo["BodyComponent"]
			.fields = keys.view()
						  .select([](std::string_view key) { return Field{key}; })
						  .to<Util::Vector>();
	}

	componentInitializerFactories["SpriteComponent"] = [](const json& j) -> ComponentInitializer {
		struct {
			std::string name;
			const sf::Sprite* sprite{};
			SpriteInfo info;

			void operator()(const GameCtx& ctx, myecs::entity e) {
				if (!sprite) {
					sprite = &SpriteMgr::getSprite(name);
				}
				ctx.reg.emplace<SpriteComponent>(e, info, *sprite);
			}
		} ret;
		json_init_field(ret.name, j, "sprite");
		json_init_field(ret.info.followPosition, j, "followPosition");
		json_init_field(ret.info.followAngle, j, "followAngle");
		json_init_field(ret.info.dynamicScale, j, "dynamicScale");
		json_init_field(ret.info.rotationOffset, j, "rotationOffset");
		json_init_field(ret.info.positionOffset, j, "positionOffset");

		return ret;
	};

	{
		Util::Vector<std::string_view> keys = {
			"sprite",
			"followPosition",
			"followAngle",
			"dynamicScale",
			"rotationOffset",
			"positionOffset",
		};

		componentMetaInfo["SpriteComponent"]
			.fields = keys.view()
						  .select([](std::string_view key) { return Field{key}; })
						  .to<Util::Vector>();
	}

	componentInitializerFactories["LifetimeComponent"] = [](const json& j) -> ComponentInitializer {
		float lifetime{};
		json_init_field(lifetime, j, "lifetime");
		return [=](const GameCtx& ctx, myecs::entity e) { ctx.reg.emplace<LifetimeComponent>(e).lifeTimer.set(lifetime).start(); };
	};

	{
		Util::Vector<std::string_view> keys = {
			"lifetime",
		};

		componentMetaInfo["LifetimeComponent"]
			.fields = keys.view()
						  .select([](std::string_view key) { return Field{key}; })
						  .to<Util::Vector>();
	}
}
