#include"ComponentMeta.h"
#include"CustomFieldParser.h"
#include"src/game/Components/PhysicsComponents.h"
#include"EnumMeta.h"
#include"src/game/Components/Render/RenderComponent.h"
#include"src/game/GameContext.h"
#include <src/global/DataManager.h>
#include <src/global/SpriteManager.h>
#include"src/game/Services/PhysicsBodyService.h"


template<class T>
void json_init_enum(T& e, std::string_view name, const json& j, std::string_view key) {
	auto s = json_parse<std::string>(j, key);
	if (!s)return;
	auto ee = EnumMeta::try_get<T>(name, *s);
	if (ee) {
		e = *ee;
	}
}

ComponentMeta::ComponentMeta() {
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
		//Logger::info("Density = {}", arg.density);
		return [=](GameCtx& ctx, myecs::entity e) {
			ctx.reg.get_or_emplace<BodyComponent>(e);
			PhysicsBodyService().createBody(ctx, e, arg);
		};
	};

	componentInitializerFactories["SpriteComponent"] = [](const json& j) -> ComponentInitializer {
		struct {
			std::string name;
			sf::Sprite* sprite{};
			SpriteInfo info;

			void operator()(GameCtx& ctx, myecs::entity e) {
				auto& sc = ctx.reg.emplace<SpriteComponent>(e);
				if (!sprite) {
					sprite = SpriteMgr::getSprite(name);
				}
				sc.sprite = sprite;
				sc.info = &info;
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

	componentInitializerFactories["LifetimeComponent"] = [](const json& j)->ComponentInitializer {
		float lifetime{};
		json_init_field(lifetime, j, "lifetime");
		return [=](GameCtx& ctx, myecs::entity e) {
			ctx.reg.emplace<LifetimeComponent>(e).lifeTimer.set(lifetime).start();
		};
	};
}
