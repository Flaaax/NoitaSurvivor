#include "RenderSystem.h"
#include "../Components/Render/RenderComponent.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/GameContext.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Wands/Wand.h"
#include "src/game/render/GameRenderScales.h"
#include "src/ui/render/NPainter.h"
#include "src/ui/render/NRenderBuffer.h"
#include "src/ui/render/NWindowView.h"
#include "src/ui/shapes/NLineShape.h"

namespace flx::game {
	void RenderSystem::debugRender(const GameCtx& ctx, const ui::Painter& rdr) {
		ui::NLineShape shape;
		for (const auto& e : ctx.gameState.borders) {
			if (const auto b = ctx.reg.try_get<BodyComponent>(e)) {
				auto [point1, point2] = b2Shape_GetSegment(b->shape);
				shape.set(point1, point2, {200, 0, 0});
				rdr.draw(shape);
			}
		}
	}

	void RenderSystem::update(const GameCtx& ctx, float dt) {
		for (const auto& [e, tc] : ctx.reg.view<SpriteEffectComponent>()) {
			for (const auto& effect : tc.effectList) {
				effect->update(dt);
			}
		}
	}

	void RenderSystem::render(ui::RenderBuffer& buffer, const GameCtx& ctx) {
		using namespace myecs;

		ctx.scales.offset = ctx.appCtx.windowViewport.canvasSize / 2.f;

		// rdr.updateGameRender(ctx.gameState.cameraPos * NWindow::viewport.gameRenderScale - NWindow::viewport.gameRenderOffset);

		buffer.clear({100, 100, 100});

		const vec2 offset = ctx.scales.offset - ctx.gameState.cameraPos * ctx.scales.scale;

		ui::Painter painter(buffer);
		painter.states.transform
			.translate(offset)
			.scale(ctx.scales.scale);

		// 还不支持Camera

		sf::RectangleShape arenaShape;
		arenaShape.setSize(ctx.gameState.bound.size);
		arenaShape.setFillColor(sf::Color(170, 170, 170));
		arenaShape.setPosition(ctx.gameState.bound.position);
		painter.draw(arenaShape);

		if (ctx.gameState.debugMode) {
			sf::RectangleShape testShape;
			testShape.setSize({4, 4});
			testShape.setFillColor({100, 100, 100});
			testShape.setPosition({0, 0});
			painter.draw(testShape);
		}

		constexpr u64 layerCount = static_cast<u64>(RenderLayer::Count);

		Array<Vector<Pair<entity, const SpriteComponent&>>, layerCount> renderObjects{};

		for (const auto& [e, c] : ctx.reg.view<SpriteComponent>()) {
			renderObjects[c.options.layer].emplace_back(e, c);
		}

		for (auto& layer : renderObjects) {
			for (auto& [e, c] : layer) {
				sf::Sprite sprite{c.texture};
				PhysicsService ps{};

				const auto bc = ctx.reg.try_get<BodyComponent>(e);

				const vec2 size = static_cast<vec2>(sprite.getTexture().getSize());
				if (c.options.targetSize != vec2{}) {
					sprite.setScale(c.options.targetSize / size);
				}
				if (c.options.centerAlinged) {
					sprite.setOrigin(size / 2.f);
				}
				sprite.scale(c.options.scale);
				if (c.options.followPosition) {
					const vec2 pos = bc ? ps.getPosition(*bc) : c.position;
					sprite.setPosition(pos + c.options.offset);
				}
				if (bc && c.options.followRotation) {
					sprite.setRotation(sf::radians(ps.getRotation(*bc)) + sf::degrees(c.options.rotation));
				}
				if (bc && c.options.dynamicScale) {
					float r = ps.getRadius(*bc);
					sprite.scale({r, r});
				}

				if (const auto sec = ctx.reg.try_get<SpriteEffectComponent>(e)) {
					auto& el = sec->effectList;
					for (auto it = el.begin(); it != el.end();) {
						(*it)->apply(sprite);

						if ((*it)->isDone()) {
							it = el.erase(it);
						} else
							++it;
					}
				}

				painter.draw(sprite);
			}
		}

		ctx.gameState.wandManager.draw(painter);
		// for (const auto& wand : ctx.gameState.wands) {
		// 	wand->draw(painter);
		// }

		if (ctx.gameState.debugMode) {
			debugRender(ctx, painter);
		}
	}
} // namespace flx::game