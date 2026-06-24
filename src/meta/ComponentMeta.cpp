#include "ComponentMeta.h"
#include "CustomFieldParser.h"
// #include "EnumMeta.h"
#include "src/app/global/Loader.h"
#include "src/app/global/SpriteManager.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/Components/Render/RenderComponent.h"
#include "src/game/GameContext.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Container/View.h"
#include "src/utils/Fon/Fon.h"
#include "src/utils/Fon/FonExt.h"

namespace flx::meta {
	using namespace game;

	ComponentMeta::ComponentMeta() {
		initGeneratedMetaInfo();
		initGeneratedComponentInitializers();
		initCustomComponentInitializers();
	}

	void ComponentMeta::initCustomComponentInitializers() {
		componentInitializerFactories["BodyComponent"] = [](const Fon& j) -> ComponentInitializer {
			BodyArg arg;
			initField(arg.type, j, "type");
			initField(arg.fixedRotation, j, "fixedRotation");
			initField(arg.shape, j, "shape", true);
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
			Vector<std::string_view> keys = {
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
				.fields = view::all(keys)
							  .select([](std::string_view key) { return Field{key}; })
							  .to<Vector>();
		}

		componentInitializerFactories["SpriteComponent"] = [](const Fon& j) -> ComponentInitializer {
			struct {
				std::string entry;
				const sf::Texture* texture{};
				SpriteRenderOptions options;

				void operator()(const GameCtx& ctx, myecs::entity e) {
					if (!texture) {
						texture = app::Loader::loadTexture(entry, true);
					}
					ctx.reg.emplace<SpriteComponent>(e, *texture, options);
				}
			} ret;

			initField(ret.entry, j, "texture");
			initField(ret.options.followPosition, j, "followPosition");
			initField(ret.options.followRotation, j, "followRotation");
			initField(ret.options.dynamicScale, j, "dynamicScale");
			initField(ret.options.rotation, j, "rotation");
			initField(ret.options.offset, j, "offset");
			initField(ret.options.targetSize, j, "targetSize");
			initField(ret.options.layer, j, "layer");

			if (const auto scale = fon::getIfVec2(j, "scale")) {
				ret.options.scale = *scale;
			} else if (const auto scale1 = j.getIf<float>("scale")) {
				ret.options.scale = {*scale1, *scale1};
			}

			return ret;
		};

		{
			Vector<std::string_view> keys = {
				"texture",
				"followPosition",
				"followRotation",
				"dynamicScale",
				"rotation",
				"offset",
				"targetSize",
				"scale",
				"layer",
			};

			componentMetaInfo["SpriteComponent"]
				.fields = view::all(keys)
							  .select([](std::string_view key) { return Field{key}; })
							  .to<Vector>();
		}

		componentInitializerFactories["LifetimeComponent"] = [](const Fon& j) -> ComponentInitializer {
			float lifetime{};
			initField(lifetime, j, "lifetime");
			return [=](const GameCtx& ctx, myecs::entity e) {
				ctx.reg.emplace<LifetimeComponent>(e).lifeTimer.set(lifetime).start();
			};
		};

		{
			Vector<std::string_view> keys = {
				"lifetime",
			};

			componentMetaInfo["LifetimeComponent"]
				.fields = view::all(keys)
							  .select([](std::string_view key) { return Field{key}; })
							  .to<Vector>();
		}
	}
} // namespace flx::meta