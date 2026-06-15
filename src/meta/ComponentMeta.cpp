#include "ComponentMeta.h"
#include "CustomFieldParser.h"
// #include "EnumMeta.h"
#include "src/app/global/SpriteManager.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Components/Render/RenderComponent.h"
#include "src/game/GameContext.h"
#include "src/game/Services/PhysicsService.h"

namespace flx::meta {
	using namespace game;

	ComponentMeta::ComponentMeta() {
		initGeneratedMetaInfo();
		initGeneratedComponentInitializers();
		initCustomComponentInitializers();
	}

	void ComponentMeta::initCustomComponentInitializers() {
		componentInitializerFactories["BodyComponent"] = [](const Json& j) -> ComponentInitializer {
			BodyArg arg;
			initField(arg.type, j, "type");
			initField(arg.fixedRotation, j, "fixedRotation");
			initField(arg.shape, j, "shape");
			initField(arg.size, j, "size");
			initField(arg.radius, j, "radius");
			initField(arg.density, j, "density");
			initField(arg.friction, j, "friction");
			initField(arg.restitution, j, "restitution");
			initField(arg.isSensor, j, "isSensor");
			// Logger::info("Density = {}", arg.density);
			return [=](const GameCtx& ctx, myecs::entity e) {
				ctx.reg.emplace<BodyComponent>(e);
				PhysicsService().createBody(ctx, e, arg);
				// Logger::info("Created body for entity {}", e.string());
				// Logger::info("Entity {} has BodyComponent: {}", e.string(), ctx.reg.has<BodyComponent>(e));
			};
		};

		{
			flx::Vector<std::string_view> keys = {
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
							  .to<Vector>();
		}

		componentInitializerFactories["SpriteComponent"] = [](const Json& j) -> ComponentInitializer {
			struct {
				std::string name;
				const sf::Sprite* sprite{};
				SpriteRenderOptions info;

				void operator()(const GameCtx& ctx, myecs::entity e) {
					if (!sprite) {
						sprite = &app::SpriteMgr::getSprite(name);
					}
					ctx.reg.emplace<SpriteComponent>(e, info, *sprite);
				}
			} ret;
			initField(ret.name, j, "sprite");
			initField(ret.info.followPosition, j, "followPosition");
			initField(ret.info.followAngle, j, "followAngle");
			initField(ret.info.dynamicScale, j, "dynamicScale");
			initField(ret.info.rotationOffset, j, "rotationOffset");
			initField(ret.info.positionOffset, j, "positionOffset");

			return ret;
		};

		{
			flx::Vector<std::string_view> keys = {
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
							  .to<Vector>();
		}

		componentInitializerFactories["LifetimeComponent"] = [](const Json& j) -> ComponentInitializer {
			float lifetime{};
			initField(lifetime, j, "lifetime");
			return [=](const GameCtx& ctx, myecs::entity e) { ctx.reg.emplace<LifetimeComponent>(e).lifeTimer.set(lifetime).start(); };
		};

		{
			flx::Vector<std::string_view> keys = {
				"lifetime",
			};

			componentMetaInfo["LifetimeComponent"]
				.fields = keys.view()
							  .select([](std::string_view key) { return Field{key}; })
							  .to<Vector>();
		}
	}
} // namespace flx::meta