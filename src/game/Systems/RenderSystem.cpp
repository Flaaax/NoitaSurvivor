#include "RenderSystem.h"
#include "../Components/Render/RenderComponent.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Wands/Wand.h"
#include "src/global/DataManager.h"
#include "src/gui/Renderer.h"
#include "src/gui/shapes/NLineShape.h"
#include <src/game/Components/PhysicsComponents.h>

void RenderSystem::debugRender(const GameCtx& ctx, Renderer& rdr) {
	NLineShape shape;
	for (const auto& e : ctx.gameState.borders) {
		if (const auto b = ctx.reg.try_get<BodyComponent>(e)) {
			auto [point1, point2] = b2Shape_GetSegment(b->shape);
			shape.set(point1, point2, {200, 0, 0});
			rdr.drawGame(shape);
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

void RenderSystem::render(Renderer& rdr, const GameCtx& ctx) {
	rdr.updateGameRender(ctx.gameState.cameraPos * NWindow::scale.gameRenderScale - NWindow::scale.gameRenderOffset);

	rdr.clear(sf::Color(100, 100, 100));

	sf::RectangleShape arenaShape;
	arenaShape.setSize(ctx.gameState.bound.size);
	arenaShape.setFillColor(sf::Color(170, 170, 170));
	arenaShape.setPosition(ctx.gameState.bound.position);
	rdr.drawGame(arenaShape);

	if (ctx.gameState.debugMode) {
		sf::RectangleShape testShape;
		testShape.setSize({4, 4});
		testShape.setFillColor({100, 100, 100});
		testShape.setPosition({0, 0});
		rdr.drawGame(testShape);
	}

	// Logger::info("begin render");
	for (const auto& [e, c, bc] : ctx.reg.view<SpriteComponent, BodyComponent>()) {
		sf::Sprite copy = c.sprite;
		PhysicsService ps{};

		if (c.info.followPosition)
			copy.setPosition(ps.getPosition(bc) + c.info.positionOffset);
		if (c.info.followAngle)
			copy.setRotation(sf::radians(ps.getRotation(bc)) + sf::degrees(c.info.rotationOffset));
		if (c.info.dynamicScale) {
			float r = ps.getRadius(bc);
			copy.scale({r, r});
		}

		if (const auto tc = ctx.reg.try_get<SpriteEffectComponent>(e)) {
			auto& ts = tc->effectList;
			for (auto it = ts.begin(); it != ts.end();) {
				(*it)->apply(copy);

				if ((*it)->isDone()) {
					it = ts.erase(it);
				} else
					++it;
			}
		}

		rdr.drawGame(copy);
	}

	for (const auto& wand : ctx.gameState.wands) {
		wand->render(rdr);
	}

	if (ctx.gameState.debugMode) {
		debugRender(ctx, rdr);
	}
}
