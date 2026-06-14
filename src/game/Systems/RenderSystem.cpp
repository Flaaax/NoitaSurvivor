#include "RenderSystem.h"
#include "../../app/global/DataManager.h"
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
	void RenderSystem::debugRender(const GameCtx& ctx, const ui::NPainter& rdr) {
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

	void RenderSystem::render(ui::NRenderBuffer& buffer, const GameCtx& ctx) {
		ctx.scales.offset = ctx.appCtx.windowViewport.canvasSize / 2.f;

		// rdr.updateGameRender(ctx.gameState.cameraPos * NWindow::viewport.gameRenderScale - NWindow::viewport.gameRenderOffset);

		buffer.clear({100, 100, 100});

		const vec2 offset = ctx.scales.offset - ctx.gameState.cameraPos * ctx.scales.scale;

		ui::NPainter painter(buffer);
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

		// Logger::info("begin render");
		for (const auto& [e, c] : ctx.reg.view<SpriteComponent>()) {
			sf::Sprite copy = c.sprite;
			PhysicsService ps{};

			const auto bc = ctx.reg.try_get<BodyComponent>(e);

			if (c.info.followPosition) {
				const vec2 pos = bc ? ps.getPosition(*bc) : c.position;
				copy.setPosition(pos + c.info.positionOffset);
			}
			if (c.info.followAngle) {
				copy.setRotation(sf::radians(ps.getRotation(*bc)) + sf::degrees(c.info.rotationOffset));
			}
			if (c.info.dynamicScale) {
				float r = ps.getRadius(*bc);
				copy.scale({r, r});
			}

			if (const auto sec = ctx.reg.try_get<SpriteEffectComponent>(e)) {
				auto& el = sec->effectList;
				for (auto it = el.begin(); it != el.end();) {
					(*it)->apply(copy);

					if ((*it)->isDone()) {
						it = el.erase(it);
					} else
						++it;
				}
			}

			painter.draw(copy);
		}

		for (const auto& wand : ctx.gameState.wands) {
			wand->render(painter);
		}

		if (ctx.gameState.debugMode) {
			debugRender(ctx, painter);
		}
	}
} // namespace flx::game